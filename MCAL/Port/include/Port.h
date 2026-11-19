#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"

typedef uint16_t Port_PinType;

typedef enum {
    PORT_PIN_IN = 0,
    PORT_PIN_OUT
} Port_PinDirectionType;

typedef uint8_t Port_PinModeType;

typedef struct {
    Port_PinType            PinID;
    Port_PinDirectionType   Direction;
    uint8_t                 Mode;
    uint8_t                 OutputType;
    uint8_t                 PullResistor;
    uint8_t                 Speed;
    uint8_t                 AlternateFunc;
    uint8_t                 InitialLevel;
} Port_PinConfigType;

typedef struct {
    uint16_t                    NumberOfPins;
    const Port_PinConfigType*   Pins;
} Port_ConfigType;

void Port_Init(const Port_ConfigType* ConfigPtr);

#endif // PORT_H