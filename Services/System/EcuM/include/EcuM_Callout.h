#ifndef ECUM_CBK_H
#define ECUM_CBK_H

#include "EcuM_PBcfg.h"

EcuM_ConfigType* EcuM_DeterminePbConfiguration(void);
void EcuM_AL_DriverInitZero(void);
void EcuM_AL_DriverInitOne(const EcuM_ConfigType* ConfigPtr);

#endif // ECUM_CBK_H