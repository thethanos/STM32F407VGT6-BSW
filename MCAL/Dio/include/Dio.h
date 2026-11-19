#ifndef DIO_H
#define DIO_H

#include "Std_Types.h"

typedef uint16_t    Dio_ChannelType;
typedef uint8_t     Dio_PortType;
typedef uint16_t    Dio_PortLevelType;
typedef uint8_t     Dio_LevelType;

typedef struct {
    uint16_t        Mask;
    uint8_t         Offset;
    Dio_PortType    Port;
} Dio_ChannelGroupType;

Dio_LevelType       Dio_ReadChannel(Dio_ChannelType ChannelID);

void                Dio_WriteChannel(Dio_ChannelType ChannelID, Dio_LevelType Level);

Dio_PortLevelType   Dio_ReadPort(Dio_PortType PortID);

void                Dio_WritePort(Dio_PortType PortID, Dio_PortLevelType Level);

Dio_LevelType       Dio_FlipChannel(Dio_ChannelType ChannelID);

#endif // DIO_H