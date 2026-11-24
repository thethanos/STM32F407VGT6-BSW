#ifndef PORT_PB_CFG_H
#define PORT_PB_CFG_H

typedef uint8 Port_PinType;

typedef struct {
    Port_PinType    PinId;
} Port_PinConfigType;

typedef struct {
    uint16                      NumberOfPortPins;
    const Port_PinConfigType*   Pins;
} Port_ConfigType;

#endif // PORT_PB_CFG_H