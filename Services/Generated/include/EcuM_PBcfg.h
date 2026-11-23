#ifndef ECUM_PB_CFG_H
#define ECUM_PB_CFG_H

#include "Mcu_PBcfg.h"

typedef struct {
    const Mcu_ConfigType* McuConfigPtr;
    Mcu_ClockType StartupClockSetting;
    uint32 ConfigConsistencyHash;
} EcuM_ConfigType;

extern const EcuM_ConfigType EcuM_Config;

#endif // ECUM_PB_CFG_H