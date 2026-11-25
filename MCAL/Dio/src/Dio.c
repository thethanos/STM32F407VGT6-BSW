#include "Dio.h"
#include "Std_Types.h"
#include "stm32f407xx.h"
#include <stdint.h>

#define DIO_GET_PORT_ADDR(PortID)   ((GPIO_TypeDef*)(GPIOA_BASE + (0x0400U * (PortID))))
#define DIO_GET_PIN_MASK(ChannelID) ((uint16_t)(1U << ((ChannelID) % 16U))) // FIXED (% instead of /)
#define DIO_GET_PORT_ID(ChannelID)  ((uint8_t)((ChannelID) / 16U))

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelID) {
    GPIO_TypeDef* port = DIO_GET_PORT_ADDR(DIO_GET_PORT_ID(ChannelID));
    uint16_t pinMask = DIO_GET_PIN_MASK(ChannelID);

    if ((port->IDR & pinMask) != 0U) {
        return STD_HIGH;
    }
    return STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType ChannelID, Dio_LevelType Level) {
    GPIO_TypeDef* port = DIO_GET_PORT_ADDR(DIO_GET_PORT_ID(ChannelID));
    uint16_t pinMask = DIO_GET_PIN_MASK(ChannelID);

    if (Level == STD_HIGH) {
        port->BSRR = pinMask;
    } else {
        port->BSRR = ((uint32_t)pinMask << 16U);
    }
}

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortID) {
    GPIO_TypeDef* port = DIO_GET_PORT_ADDR(PortID);
    return (Dio_PortLevelType)(port->IDR);
}

void Dio_WritePort(Dio_PortType PortID, Dio_PortLevelType Level) {
    GPIO_TypeDef* port = DIO_GET_PORT_ADDR(PortID);
    port->ODR = Level;
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelID) {
    GPIO_TypeDef* port = DIO_GET_PORT_ADDR(DIO_GET_PORT_ID(ChannelID));
    uint16_t pinMask = DIO_GET_PIN_MASK(ChannelID);

    if ((port->ODR & pinMask) != 0U) {
        port->BSRR = ((uint32_t)pinMask << 16U);
        return STD_LOW;
    } else {
        port->BSRR = pinMask;
        return STD_HIGH;
    }
}
