#include "EcuM_Callout.h"

#include "EcuM_PBcfg.h"
#include "Mcu.h"

EcuM_ConfigType* EcuM_DeterminePbConfiguration(void) {
    return &EcuM_Config;
}

void EcuM_AL_DriverInitZero(void) {}

void EcuM_AL_DriverInitOne(const EcuM_ConfigType* ConfigPtr) {
    if ((ConfigPtr == NULL_PTR) || (ConfigPtr->McuConfigPtr == NULL_PTR)) {
        // report error
        return;
    }

    Mcu_Init(ConfigPtr->McuConfigPtr);

#if (MCU_INIT_CLOCK == STD_ON)
    if (Mcu_InitClock(ConfigPtr->StartupClockSetting) != E_OK) {
        // report error
        return;
    }
#endif

#if (MCU_NO_PLL == STD_OFF)
    while (Mcu_GetPllStatus() != MCU_PLL_LOCKED);
    Mcu_DistributePllClock();
#endif
}