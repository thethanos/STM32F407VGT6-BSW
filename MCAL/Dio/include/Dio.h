#ifndef DIO_H
#define DIO_H

#include "Dio_PBcfg.h"

#define DIO_READCHANNEL_SID             ((uint8)0x00)
#define DIO_WRITECHANNEDL_SID           ((uint8)0x01)

#define DIO_E_PARAM_INVALID_CHANNEL_ID  ((uint8)0x0A)
#define DIO_E_PARAM_INVALID_PORT_ID     ((uint8)0x14)

Dio_LevelType       Dio_ReadChannel(Dio_ChannelType ChannelID);
void                Dio_WriteChannel(Dio_ChannelType ChannelID, Dio_LevelType Level);
Dio_PortLevelType   Dio_ReadPort(Dio_PortType PortID);
void                Dio_WritePort(Dio_PortType PortID, Dio_PortLevelType Level);
Dio_LevelType       Dio_FlipChannel(Dio_ChannelType ChannelID);

#endif // DIO_H