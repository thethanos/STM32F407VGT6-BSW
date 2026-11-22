#ifndef MCU_H
#define MCU_H

#include "Std_Types.h"
#include "Mcu_Cfg.h"

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

typedef uint8 Mcu_ClockType;

typedef struct {
    uint8   McuClockSettingId;
    uint32  PllSource;
    uint32  PllM;
    uint32  PllN;
    uint32  PllP;
    uint32  PllQ;
    uint32  AhbPrescaler;
    uint32  Apb1Prescaler;
    uint32  Apb2Prescaler;
    uint32  FlashLatency;
    boolean UsePll;
    uint32  McuClockReferencePointFrequency;
} Mcu_ClockSettingConfigType;

typedef enum {
    MCU_RUN = 0U,
    MCU_SLEEP,
    MCU_STOP,
    MCU_STANDBY
} Mcu_LowPowerModeType;

typedef struct {
    uint8                   ModeId;
    Mcu_LowPowerModeType    PowerMode;
    uint32                  PwrControlMask;
    boolean                 SleepOnExit;
} Mcu_ModeSettingConfigType;

typedef struct {
    uint32 RamBaseAddress;
    uint32 RamSizeBytes;
    uint8  DefaultValue;
    uint8  WriteSize; 
} Mcu_RamSectorSettingConfigType;

typedef struct {
    uint32  PwrControlInit;
    uint32  FlashAcrInit;
    boolean ClockFailureNotificationEnabled;

    uint8 NumClockSettings;
    const Mcu_ClockSettingConfigType* ClockSettings;

    uint8 NumModeSettings;
    const Mcu_ModeSettingConfigType* ModeSettings;

    uint8 NumRamSectorSettings;
    const Mcu_RamSectorSettingConfigType* RamSectorSettings;

    boolean EnablePerformResetApi;
} Mcu_ConfigType;


typedef enum {
    MCU_PLL_LOCKED = 0x00,
    MCU_PLL_UNLOCKED = 0x01,
    MCU_PLL_STATUS_UNDEFINED = 0x02
} Mcu_PllStatusType;

typedef uint32 Mcu_RawResetType;

typedef enum {
    MCU_POWER_ON_RESET = 0U,
    MCU_WATCHDOG_RESET,
    MCU_SW_RESET,
    MCU_RESET_UNDEFINED
} Mcu_ResetType;

typedef uint8 Mcu_RamSectionType;

typedef uint8 Mcu_ModeType;

typedef enum {
    MCU_RAMSTATE_INVALID    = 0x00,
    MCU_RAMSTATE_VALID      = 0x01
} Mcu_RamStateType; 

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
