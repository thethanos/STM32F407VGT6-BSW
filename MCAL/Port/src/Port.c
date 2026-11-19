#include "Port.h"
#include "stm32f4xx.h"

#include <stddef.h>

#define GET_GPIO_PORT(pin)  ((GPIO_TypeDef*)(GPIOA_BASE + (0x0400U * ((pin) / 16U))))
#define GET_GPIO_PIN(pin)   ((pin) % 16U)

void Port_Init(const Port_ConfigType *ConfigPtr) {
    if (ConfigPtr == NULL || ConfigPtr->Pins == NULL) {
        return;
    }

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | 
                    RCC_AHB1ENR_GPIOEEN;

    for (uint16_t i = 0U; i < ConfigPtr->NumberOfPins; i++) {
        const Port_PinConfigType* pinCfg = &ConfigPtr->Pins[i];

        GPIO_TypeDef* port = GET_GPIO_PORT(pinCfg->PinID);
        uint8_t pin = GET_GPIO_PIN(pinCfg->PinID);

        if (pinCfg->Direction == PORT_PIN_OUT) {
            if (pinCfg->InitialLevel == STD_HIGH) {
                port->BSRR = (1U << pin);
            } else {
                port->BSRR = (1U << (pin + 16U));
            }
        }

        port->MODER &= ~(0x03U << (pin * 2U));
        port->MODER |= ((uint32_t)pinCfg->Mode << (pin * 2U));

        port->OTYPER &= ~(0x01U << pin);
        port->OTYPER |= ((uint32_t)pinCfg->OutputType << pin);

        port->OSPEEDR &= ~(0x03U << (pin * 2U));
        port->OSPEEDR |= ((uint32_t)pinCfg->Speed << (pin * 2U));

        port->PUPDR &= ~(0x03U << (pin * 2U));
        port->PUPDR |= ((uint32_t)pinCfg->PullResistor << (pin * 2U));

        if (pinCfg->Mode == 2U) {
            uint8_t afrIndex = pin / 8U;
            uint8_t afrShift = (pin % 8U) * 4U;
            port->AFR[afrIndex] &= ~(0x0FU << afrShift);
            port->AFR[afrIndex] |= ((uint32_t)pinCfg->AlternateFunc << afrShift);
        }
    }
}