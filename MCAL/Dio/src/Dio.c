#include "Dio.h"
#include "Det.h"
#include "Gpio_Hw.h"

#define GPIO_BSRR_RESET_SHIFT 16U

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelID) {
    Dio_LevelType level = STD_LOW;
    if (GpioHw_IsPinIdValid(ChannelID) == FALSE) {
#if (DIO_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_READCHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
#endif
        return STD_LOW;
    }

    GPIO_TypeDef* const port = GpioHw_Port(GpioHw_PortIdx(ChannelID));
    uint32 pinMask = GpioHw_PinMask(ChannelID);

    if ((port->IDR & pinMask) != 0U) {
        return STD_HIGH;
    }
    return STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType ChannelID, Dio_LevelType Level) {
    if (GpioHw_IsPinIdValid(ChannelID) == FALSE) {
#if (DIO_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_WRITECHANNEDL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
#endif
        return;
    }

    GPIO_TypeDef* const port = GpioHw_Port(GpioHw_PortIdx(ChannelID));
    const uint32 pinMask = GpioHw_PinMask(ChannelID);

    if (Level == STD_HIGH) {
        port->BSRR = pinMask;
        return;
    }
    
    port->BSRR = (pinMask << GPIO_BSRR_RESET_SHIFT);
}

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortID) {
    GPIO_TypeDef* port = GpioHw_Port(PortID);
    return (Dio_PortLevelType)(port->IDR);
}

void Dio_WritePort(Dio_PortType PortID, Dio_PortLevelType Level) {
    GPIO_TypeDef* port = GpioHw_Port(PortID);
    port->ODR = Level;
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelID) {
    GPIO_TypeDef* port = GpioHw_Port(GpioHw_PortIdx(ChannelID));
    uint32 pinMask = GpioHw_PinMask(ChannelID);

    if ((port->ODR & pinMask) != 0U) {
        port->BSRR = (pinMask << 16U);
        return STD_LOW;
    } else {
        port->BSRR = pinMask;
        return STD_HIGH;
    }
}
