#include "EcuM.h"
#include "EcuM_PBcfg.h"

static const EcuM_ConfigType* EcuM_ActiveConfigPtr = NULL_PTR;

void EcuM_Init(void) {
    EcuM_AL_DriverInitZero();

    EcuM_ActiveConfigPtr = EcuM_DeterminePbConfiguration();
    if (EcuM_ActiveConfigPtr == NULL_PTR) {
        //report error
        return;
    }

    EcuM_AL_DriverInitOne(EcuM_ActiveConfigPtr);

    // to be continued
}