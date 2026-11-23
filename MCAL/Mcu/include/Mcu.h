#ifndef MCU_H
#define MCU_H

#include "Std_Types.h"
#include "Mcu_Cfg.h"
#include "Mcu_PBcfg.h"

#define MCU_VENDOR_ID                   ((uint16)0xFFFFU)
#define MCU_AR_RELEASE_MAJOR_VERSION    4U
#define MCU_AR_RELEASE_MINOR_VERSION    4U
#define MCU_AR_RELEASE_REVISION_VERSION 0U
#define MCU_SW_MAJOR_VERSION            ((uint8)1)
#define MCU_SW_MINOR_VERSION            ((uint8)0)
#define MCU_SW_PATCH_VERSION            ((uint8)0)

#define MCU_INIT_SID                ((uint8)0x00)
#define MCU_INITRAMSECTION_SID      ((uint8)0x01)
#define MCU_INITCLOCK_SID           ((uint8)0x02)
#define MCU_DISTRIBUTEPLLCLOCK_SID  ((uint8)0x03)
#define MCU_GETPLLSTATUS_SID        ((uint8)0x04)
#define MCU_GETRESETREASON_SID      ((uint8)0x05)
#define MCU_GETRESETRAWVALUE_SID    ((uint8)0x06)
#define MCU_PERFORMRESET_SID        ((uint8)0x07)
#define MCU_SETMODE_SID             ((uint8)0x08)
#define MCU_GETVERSIONINFO_SID      ((uint8)0x09)
#define MCU_GETRAMSTATE_SID         ((uint8)0x0A)

#define MCU_E_PARAM_CONFIG      ((uint8)0x0A)
#define MCU_E_PARAM_CLOCK       ((uint8)0x0B)
#define MCU_E_PARAM_MODE        ((uint8)0x0C)
#define MCU_E_PARAM_RAMSECTION  ((uint8)0x0D)
#define MCU_E_PLL_NOT_LOCKED    ((uint8)0x0E)
#define MCU_E_UNINIT            ((uint8)0x0F)
#define MCU_E_PARAM_POINTER     ((uint8)0x10)
#define MCU_E_INIT_FAILED       ((uint8)0x11)

void Mcu_Init(const Mcu_ConfigType* ConfigPtr);

#if (MCU_INIT_CLOCK == STD_ON)
Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting);
#endif

Mcu_PllStatusType Mcu_GetPllStatus(void);

#if (MCU_NO_PLL == STD_OFF)
Std_ReturnType Mcu_DistributePllClock(void);
#endif

Mcu_RawResetType Mcu_GetResetRawValue(void);

Mcu_ResetType Mcu_GetResetReason(void);

Std_ReturnType Mcu_InitRamSection(Mcu_RamSectionType RamSection);

#if (MCU_PERFORM_RESET_API == STD_ON)
void Mcu_PerformReset(void);
#endif

void Mcu_SetMode(Mcu_ModeType McuMode);

#if (MCU_VERSION_INFO_API == STD_ON)
void Mcu_GetVersionInfo(Std_VersionInfoType* versioninfo);
#endif

#endif // MCU_H
