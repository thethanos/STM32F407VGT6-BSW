#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"
#include "Port_PBcfg.h"

#define PORT_INIT_SID           ((uint8)0x00)

#define PORT_E_INIT_FAILED      ((uint8)0x0C)


void Port_Init(const Port_ConfigType* ConfigPtr);

#endif // PORT_H