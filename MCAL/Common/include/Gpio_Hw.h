#ifndef GPIO_HW_H
#define GPIO_HW_H

#include "Std_Types.h"
#include "stm32f407xx.h"

#define GPIO_HW_PORT_COUNT      8U
#define GPIO_HW_PINS_PER_PORT   16U

static inline uint32 GpioHw_PortIdx(uint16 id) {
    return (uint32)id >> 4U;
}

static inline boolean GpioHw_IsPortValid(uint32 portIdx) {
    return (portIdx < GPIO_HW_PORT_COUNT) ? TRUE : FALSE;
}

static inline uint32 GpioHw_PinIdx(uint16 id) {
    return (uint32)id & 0x0FU;
}

static inline uint32 GpioHw_PinMask(uint16 id) {
    return 1UL << GpioHw_PinIdx(id);
}

static inline boolean GpioHw_IsPinIdValid(uint16 id) {
    return (GpioHw_PortIdx(id) < GPIO_HW_PORT_COUNT) ? TRUE : FALSE;
}

static inline GPIO_TypeDef* GpioHw_Port(uint32 portIdx) {
    return (GPIO_TypeDef*)(GPIOA_BASE + (portIdx * (GPIOB_BASE - GPIOA_BASE)));
}

#endif // GPIO_HW_H