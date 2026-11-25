#ifndef PORT_PB_CFG_H
#define PORT_PB_CFG_H


typedef uint8 Port_PinType;
typedef uint8 Port_PinModeType;

#define PORT_PIN_MODE_DIO       ((Port_PinModeType)0x00)
#define PORT_PIN_MODE_ANALOG    ((Port_PinModeType)0x01)
#define PORT_PIN_MODE_AF(n)     ((Port_PinModeType)(0x10U | ((uint8)(n) & 0x0FU)))

typedef enum {
    PORT_PIN_IN     = 0U,
    PORT_PIN_OUT    = 1U
} Port_PinDirectionType;

typedef enum {
    PORT_PIN_LEVEL_LOW  = 0U,
    PORT_PIN_LEVEL_HIGH = 1U
} Port_PinLevelType;

typedef enum {
    PORT_PIN_PULL_NONE  = 0U,
    PORT_PIN_PULL_UP    = 1U,
    PORT_PIN_PULL_DOWN  = 2U
} Port_PinPullType;

typedef enum {
    PORT_PIN_OTYPE_PUSH_PULL    = 0U,
    PORT_PIN_OTYPE_OPEN_DRAIN   = 1U
} Port_PinOutputType;

typedef enum {
    PORT_PIN_SPEED_LOW          = 0U,
    PORT_PIN_SPEED_MEDIUM       = 1U,
    PORT_PIN_SPEED_HIGH         = 2U,
    PORT_PIN_SPEED_VERY_HIGH    = 3U
} Port_PinSpeedType;

typedef struct {
    Port_PinType            PinId;
    Port_PinModeType        InitialMode;
    Port_PinDirectionType   Direction;
    boolean                 DirectionChangeable;
    Port_PinLevelType       LevelValue;
    Port_PinPullType        Pull;
    Port_PinOutputType      OutputType;
    Port_PinSpeedType       Speed;
} Port_PinConfigType;

typedef struct {
    uint16                      NumberOfPortPins;
    const Port_PinConfigType*   Pins;
} Port_ConfigType;

#endif // PORT_PB_CFG_H