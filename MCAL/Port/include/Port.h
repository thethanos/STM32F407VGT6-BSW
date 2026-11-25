#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"
#include "Port_PBcfg.h"

#define PORT_INIT_SID                       ((uint8)0x00)
#define PORT_REFRESHPORTDIRECTION_SID       ((uint8)0x02)

#define PORT_E_INIT_FAILED      ((uint8)0x0C)
#define PORT_E_UNINIT           ((uint8)0x0F)


void Port_Init(const Port_ConfigType* ConfigPtr);
void Port_RefreshPortDirection(void);

#endif // PORT_H