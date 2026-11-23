#ifndef MCU_PB_CFG_H
#define MCU_PB_CFG_H

#include "Std_Types.h"

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

extern const Mcu_ConfigType Mcu_Config;

#endif // MCU_PB_CFG_H
