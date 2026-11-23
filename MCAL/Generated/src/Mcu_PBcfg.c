#include "Mcu_PBcfg.h"

/* ============================================================================
 * Generated clock configuration
 *
 * HSE       = 8 MHz
 *
 * PLLM      = 8
 * PLLN      = 336
 * PLLP      = 2
 * PLLQ      = 7
 *
 * PLL input = 8 / 8 = 1 MHz
 * VCO       = 1 * 336 = 336 MHz
 * SYSCLK    = 336 / 2 = 168 MHz
 *
 * HCLK      = 168 / 1 = 168 MHz
 * APB1      = 168 / 4 = 42 MHz
 * APB2      = 168 / 2 = 84 MHz
 *
 * Flash     = 5 WS
 * ========================================================================== */

 #include "Mcu_PBcfg.h"

/* ========================================================================
 * 1. Clock Configuration
 * Target: SYSCLK = 168 MHz from 8 MHz HSE
 *  - PLLM = 8   -> fPLL_IN = 1 MHz (8 MHz / 8)
 *  - PLLN = 336 -> fVCO = 336 MHz (1 MHz * 336)
 *  - PLLP = 2   -> SYSCLK = 168 MHz (336 MHz / 2)
 *  - PLLQ = 7   -> fPLL48 = 48 MHz (336 MHz / 7), for USB/SDIO
 * ======================================================================== */
static const Mcu_ClockSettingConfigType Mcu_ClockSettingConfigData[] =
{
    {
        .McuClockSettingId                 = 0,
        .PllSource                         = 0x00000001, /* HSE selected as PLL source */
        .PllM                              = 8,
        .PllN                              = 336,
        .PllP                              = 2,
        .PllQ                              = 7,
        .AhbPrescaler                      = 1,          /* HCLK   = 168 MHz (max) */
        .Apb1Prescaler                     = 4,          /* PCLK1  = 42 MHz  (max 42) */
        .Apb2Prescaler                     = 2,          /* PCLK2  = 84 MHz  (max 84) */
        .FlashLatency                      = 5,          /* 5 Wait States @ 168 MHz, 3.3V */
        .UsePll                            = TRUE,
        .McuClockReferencePointFrequency   = 8000000UL   /* HSE crystal frequency in Hz */
    }
};

/* ========================================================================
 * 2. Mode Configuration
 * Run mode: Main regulator ON (VOS = Scale 1 for 168 MHz)
 * ======================================================================== */
static const Mcu_ModeSettingConfigType Mcu_ModeSettingConfigData[] =
{
    {
        .ModeId           = 0,             /* MCU_MODE_RUN */
        .PwrControlMask   = 0x00004000,    /* PWR_CR VOS bit set to Scale 1 */
        .SleepOnExit      = FALSE
    }
};

/* ========================================================================
 * 3. RAM Sector Configuration
 * Defines the RAM regions available for initialization
 * ======================================================================== */
static const Mcu_RamSectorSettingConfigType Mcu_RamSectorSettingConfigData[] =
{
    {
        /* SRAM1: 112 KB @ 0x20000000 */
        .RamBaseAddress = 0x20000000UL,
        .RamSizeBytes   = 0x0001C000UL,    /* 112 * 1024 */
        .DefaultValue   = 0x00,
        .WriteSize      = 4                /* 32-bit word writes */
    },
    {
        /* SRAM2: 16 KB @ 0x2001C000 */
        .RamBaseAddress = 0x2001C000UL,
        .RamSizeBytes   = 0x00004000UL,    /* 16 * 1024 */
        .DefaultValue   = 0x00,
        .WriteSize      = 4
    },
    {
        /* CCM RAM: 64 KB @ 0x10000000 */
        .RamBaseAddress = 0x10000000UL,
        .RamSizeBytes   = 0x00010000UL,    /* 64 * 1024 */
        .DefaultValue   = 0x00,
        .WriteSize      = 4
    }
};

/* ========================================================================
 * 4. Top-Level Configuration Root
 * ======================================================================== */
const Mcu_ConfigType Mcu_Config =
{
    .PwrControlInit                    = 0x00004000,     /* VOS = Scale 1 */
    .FlashAcrInit                      = 0x00000705,     /* 5 WS + Prefetch + I-Cache + D-Cache */
    .ClockFailureNotificationEnabled   = FALSE,          /* Not supported by STM32F4 HSE CSS in this driver */

    .NumClockSettings                  = 1,
    .ClockSettings                     = Mcu_ClockSettingConfigData,

    .NumModeSettings                   = 1,
    .ModeSettings                      = Mcu_ModeSettingConfigData,

    .NumRamSectorSettings              = 3,
    .RamSectorSettings                 = Mcu_RamSectorSettingConfigData,

    .EnablePerformResetApi             = TRUE
};