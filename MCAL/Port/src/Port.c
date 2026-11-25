#include "Port.h"
#include "Port_Cfg.h"
#include "Det.h"
#include "Port_PBcfg.h"
#include "Std_Types.h"
#include "stm32f4xx.h"
#include <stdint.h>

#define PORT_GPIO_COUNT     8U
#define PORT_GPIO_IDX(id)   ((uint32)(id) >> 4U)
#define PORT_PIN_IDX(id)    ((uint32)(id) & 0x0FU)
#define PORT_GPIO(idx)      ((GPIO_TypeDef*)(GPIOA_BASE + ((idx) * (GPIOB_BASE - GPIOA_BASE))))

#define PORT_MODER_INPUT    0U
#define PORT_MODER_OUTPUT   1U
#define PORT_MODER_AF       2U
#define PORT_MODER_ANALOG   3U

static boolean Port_Initialized = FALSE;
static const Port_ConfigType* Port_ConfigPtr = NULL_PTR;

static boolean Port_IsConfigValid(const Port_ConfigType* ConfigPtr) {
    return TRUE;
}

static void Port_ConfigurePin(const Port_PinConfigType* Pin) {
    GPIO_TypeDef* const gpio = PORT_GPIO(PORT_GPIO_IDX(Pin->PinId));
    const uint32 pin = PORT_PIN_IDX(Pin->PinId);
    const uint32 pos2 = pin * 2U;
    uint32 moder = 0;

    switch (Pin->InitialMode) {
        case PORT_PIN_MODE_DIO:
            if (Pin->Direction == PORT_PIN_OUT) {
                gpio->BSRR = (Pin->LevelValue == PORT_PIN_LEVEL_HIGH) ? (1UL << pin) : (1UL << (pin + 16U));
                moder = PORT_MODER_OUTPUT;
            } else {
                moder = PORT_MODER_INPUT;
            }
            break;
        case PORT_PIN_MODE_ANALOG:
            moder = PORT_MODER_ANALOG;
            break;
        default:
            const uint32 afrIdx = pin >> 3U;
            const uint32 afrPos = (pin & 0x07U) * 4U;
            gpio->AFR[afrIdx] = (gpio->AFR[afrIdx] & ~(0x0FUL << afrPos)) |
                                ((uint32)(Pin->InitialMode & 0x0FU) << afrPos);
            moder = PORT_MODER_AF;
    }

    gpio->OTYPER  = (gpio->OTYPER  & ~(0x01UL << pin))  | ((uint32)Pin->OutputType << pin);
    gpio->OSPEEDR = (gpio->OSPEEDR & ~(0x03UL << pos2)) | ((uint32)Pin->Speed << pos2);
    gpio->PUPDR   = (gpio->PUPDR   & ~(0x03UL << pos2)) | ((uint32)Pin->Pull << pos2);
    gpio->MODER   = (gpio->MODER   & ~(0x03UL << pos2)) | (moder << pos2);
}

void Port_Init(const Port_ConfigType *ConfigPtr) {
    uint32 clockMask = 0;

    if (ConfigPtr == NULL_PTR) {
#if (PORT_DEV_ERROR_DETECT == STD_ON)
        (void) Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID, PORT_E_INIT_FAILED);
#endif 
        return;
    }

    if (Port_IsConfigValid(ConfigPtr) == FALSE) {
#if (PORT_DEV_ERROR_DETECT == STD_ON)
        (void) Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID, PORT_E_INIT_FAILED);
#endif
        return;
    }

    for (uint16 index = 0U; index < ConfigPtr->NumberOfPortPins; index++) {
        clockMask |= (1UL << PORT_GPIO_IDX(ConfigPtr->Pins[index].PinId));
    }

    // enter critical section tbd
    RCC->AHB1ENR |= clockMask;
    (void)RCC->AHB1ENR;

    // exit critical sectiob tbd

    for (uint16 index = 0U; index < ConfigPtr->NumberOfPortPins; index++) {
        // enter critical section tbd
        Port_ConfigurePin(&ConfigPtr->Pins[index]);
        // exit critical section tbd
    }

    Port_ConfigPtr = ConfigPtr;
    Port_Initialized = TRUE;
}

void Port_RefreshPortDirection(void) {
    if (Port_Initialized == FALSE) {
#if (PORT_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESHPORTDIRECTION_SID, PORT_E_UNINIT);
#endif
        return;
    }

    for (uint16 index = 0U; index < Port_ConfigPtr->NumberOfPortPins; index++) {
        const Port_PinConfigType* const cfg = &Port_ConfigPtr->Pins[index];

        if (cfg->DirectionChangeable == TRUE) {
            continue;
        }

        GPIO_TypeDef* const gpio = PORT_GPIO(PORT_GPIO_IDX(cfg->PinId));
        const uint32 pos2 = PORT_PIN_IDX(cfg->PinId) * 2U;
        const uint32 mask = 0x03UL << pos2;
        const uint32 moder = (cfg->Direction == PORT_PIN_OUT) ? PORT_MODER_OUTPUT : PORT_MODER_INPUT;

        // enter ctitical section
        const uint32 current = (gpio->MODER & mask) >> pos2;
        if ((current == PORT_MODER_INPUT) || (current == PORT_MODER_OUTPUT)) {
            gpio->MODER = (gpio->MODER & ~mask) | (moder << pos2);
        }
        // exit critical section
    }
}