#ifndef DIO_PB_CFG_H
#define DIO_PB_CFG_H

#include "Dio_Cfg.h"

typedef uint16    Dio_ChannelType;
typedef uint8     Dio_PortType;
typedef uint16    Dio_PortLevelType;
typedef uint8     Dio_LevelType;

typedef struct {
    uint16          Mask;
    uint8           Offset;
    Dio_PortType    Port;
} Dio_ChannelGroupType;

#endif // DIO_PB_CFG_H