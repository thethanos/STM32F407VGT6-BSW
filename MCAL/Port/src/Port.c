#include "Port.h"
#include "Port_Cfg.h"
#include "Det.h"
#include "Std_Types.h"
#include "stm32f4xx.h"

#define PORT_GPIO_IDX(id)   ((uint32)(id) >> 4U)

static boolean Port_Initialized = FALSE;
static const Port_ConfigType* Port_ConfigPtr = NULL_PTR;

static boolean Port_IsConfigValid(const Port_ConfigType* ConfigPtr) {
    return TRUE;
}

static void Port_ConfigurePin(const Port_PinConfigType* Pin) {
    //GPIO_TypeDef* const gpio = 
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