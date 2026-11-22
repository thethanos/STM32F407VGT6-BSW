#include "Mcu.h"
#include "Dem.h"
#include "Dem_Cfg.h"
#include "Det.h"
#include "Std_Types.h"
#include "stm32f407xx.h"
#include "Mcu_Cfg.h"

#include <string.h>

#define MCU_UNINIT       0U
#define MCU_INITIALIZED  1U
#define MCU_RESET_RAW_UNDEFINED     0xFFFFFFFFU

#define HSE_STARTUP_TIMEOUT         0x05000U
#define PLL_SHUTDOWN_TIMEOUT        0x01000U
#define FLASH_LATENCY_TIMEOUT       0x01000U
#define CLOCK_SOURCE_SWITCH_TIMEOUT 0x01000U

static uint8 Mcu_Status = MCU_UNINIT;
static boolean Mcu_ClockStarted = FALSE;
static uint32 Mcu_ResetReason;
static const Mcu_ConfigType* Mcu_ConfigPtr = NULL_PTR;

static Std_ReturnType Mcu_WaitSet(volatile uint32* reg, uint32 mask, uint32 timeout) {
    while (timeout > 0U) {
        if ((*reg & mask) == mask) {
            return E_OK;
        }
        timeout--;
    }
    return E_NOT_OK;
}

static Std_ReturnType Mcu_WaitClear(volatile uint32* reg, uint32 mask, uint32 timeout) {
    while (timeout > 0U) {
        if ((*reg & mask) == 0U) {
            return E_OK;
        }
        timeout--;
    }
    return E_NOT_OK;
}

static Std_ReturnType Mcu_WaitEqual(volatile uint32* reg, uint32 mask, uint32 expected, uint32 timeout) {
    while (timeout > 0U) {
        if ((*reg & mask) == expected) {
            return E_OK;
        }
        timeout--;
    }
    return E_NOT_OK;
}

static Std_ReturnType Mcu_EncodeHpre(uint32 divider, uint32* code) {
    switch (divider) {
        case 1U: *code = RCC_CFGR_HPRE_DIV1; break;
        case 2U: *code = RCC_CFGR_HPRE_DIV2; break; 
        case 4U: *code = RCC_CFGR_HPRE_DIV4; break;
        case 8U: *code = RCC_CFGR_HPRE_DIV8; break;   
        case 16U: *code = RCC_CFGR_HPRE_DIV16; break;
        case 64U: *code = RCC_CFGR_HPRE_DIV64; break; 
        case 128U: *code = RCC_CFGR_HPRE_DIV128; break;
        case 256U: *code = RCC_CFGR_HPRE_DIV256; break;
        case 512U: *code = RCC_CFGR_HPRE_DIV512; break;
        default: return E_NOT_OK;   
    }
    return E_OK;
}

static Std_ReturnType Mcu_EncodePpre1(uint32 divider, uint32* code) {
    switch (divider) {
        case 1U: *code = RCC_CFGR_PPRE1_DIV1; break;
        case 2U: *code = RCC_CFGR_PPRE1_DIV2; break; 
        case 4U: *code = RCC_CFGR_PPRE1_DIV4; break;
        case 8U: *code = RCC_CFGR_PPRE1_DIV8; break;   
        case 16U: *code = RCC_CFGR_PPRE1_DIV16; break;
        default: return E_NOT_OK;
    }
    return E_OK;
}

static Std_ReturnType Mcu_EncodePpre2(uint32 divider, uint32* code) {
    switch (divider) {
        case 1U: *code = RCC_CFGR_PPRE2_DIV1; break;
        case 2U: *code = RCC_CFGR_PPRE2_DIV2; break; 
        case 4U: *code = RCC_CFGR_PPRE2_DIV4; break;
        case 8U: *code = RCC_CFGR_PPRE2_DIV8; break;   
        case 16U: *code = RCC_CFGR_PPRE2_DIV16; break;
        default: return E_NOT_OK;
    }
    return E_OK;
}


void Mcu_Init(const Mcu_ConfigType* ConfigPtr) {
    if (ConfigPtr == NULL_PTR) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INIT_SID, MCU_E_INIT_FAILED);
#endif
        return;
    }

    Mcu_ResetReason = RCC->CSR;
    RCC->CSR |= RCC_CSR_RMVF;

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;

    PWR->CR = (PWR->CR & ~PWR_CR_VOS) | (ConfigPtr->PwrControlInit & PWR_CR_VOS);

    Mcu_ConfigPtr = ConfigPtr;
    Mcu_Status    = MCU_INITIALIZED;
}

#if (MCU_INIT_CLOCK == STD_ON)
Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting) {
    Mcu_ClockStarted = FALSE;

    if (Mcu_Status != MCU_INITIALIZED) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_UNINIT);
#endif
        return E_NOT_OK;
    }

    const Mcu_ClockSettingConfigType* cfg = NULL_PTR;
    for (uint8 index = 0; index < Mcu_ConfigPtr->NumClockSettings; index++) {
        if (Mcu_ConfigPtr->ClockSettings[index].McuClockSettingId == ClockSetting) {
            cfg = &Mcu_ConfigPtr->ClockSettings[index];
            break;
        }
    }

    if (cfg == NULL_PTR) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_PARAM_CLOCK);
#endif
        return E_NOT_OK;
    }


    uint32 hpre = 0, ppre1 = 0, ppre2 = 0;
    if (Mcu_EncodeHpre(cfg->AhbPrescaler, &hpre) != E_OK) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_PARAM_CONFIG);
#endif
        return E_NOT_OK;
    }

    if (Mcu_EncodePpre1(cfg->Apb1Prescaler, &ppre1) != E_OK) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_PARAM_CONFIG);
#endif
        return E_NOT_OK;
    }

    if (Mcu_EncodePpre2(cfg->Apb2Prescaler, &ppre2) != E_OK) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_PARAM_CONFIG);
#endif
        return E_NOT_OK;
    }

    uint32 pllcfgr = 0;
    pllcfgr |= (cfg->PllSource == MCU_PLL_SOURCE_HSE) ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI;
    pllcfgr |= ((cfg->PllM << RCC_PLLCFGR_PLLM_Pos) & RCC_PLLCFGR_PLLM_Msk);
    pllcfgr |= ((cfg->PllN << RCC_PLLCFGR_PLLN_Pos) & RCC_PLLCFGR_PLLN_Msk);
    uint32 pllp_encoded = (cfg->PllP >> 1U) - 1U;
    pllcfgr |= ((pllp_encoded << RCC_PLLCFGR_PLLP_Pos) & RCC_PLLCFGR_PLLP_Msk);
    pllcfgr |= ((cfg->PllQ << RCC_PLLCFGR_PLLQ_Pos) & RCC_PLLCFGR_PLLQ_Msk);

    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL) {
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
        if (Mcu_WaitEqual(&RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_HSI, CLOCK_SOURCE_SWITCH_TIMEOUT) != E_OK) {
            (void)Dem_SetEventStatus(MCU_E_CLOCK_FAILURE, DEM_EVENT_STATUS_FAILED);
            return E_NOT_OK;
        }
    }

    uint32_t expected = (cfg->FlashLatency << FLASH_ACR_LATENCY_Pos) & FLASH_ACR_LATENCY_Msk;
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | expected;
    if (Mcu_WaitEqual(&FLASH->ACR, FLASH_ACR_LATENCY_Msk, expected, FLASH_LATENCY_TIMEOUT) != E_OK) {
        Dem_SetEventStatus(MCU_E_CLOCK_FAILURE, DEM_EVENT_STATUS_FAILED);
        return E_NOT_OK;
    }

    if (cfg->PllSource == MCU_PLL_SOURCE_HSE) {
        RCC->CR |= RCC_CR_HSEON;
        if (Mcu_WaitSet(&RCC->CR, RCC_CR_HSERDY, HSE_STARTUP_TIMEOUT) != E_OK) {
            Dem_SetEventStatus(MCU_E_CLOCK_FAILURE, DEM_EVENT_STATUS_FAILED);
            return E_NOT_OK;
        }
    }

    RCC->CR &= ~RCC_CR_PLLON;
    if (Mcu_WaitClear(&RCC->CR, RCC_CR_PLLRDY, PLL_SHUTDOWN_TIMEOUT) != E_OK) {
        Dem_SetEventStatus(MCU_E_CLOCK_FAILURE, DEM_EVENT_STATUS_FAILED);
        return E_NOT_OK;
    }

    RCC->PLLCFGR = pllcfgr;
    RCC->CR |= RCC_CR_PLLON;

    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) | hpre | ppre1 | ppre2;

    Mcu_ClockStarted = TRUE;
    return E_OK;
}
#endif

Mcu_PllStatusType Mcu_GetPllStatus(void) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETPLLSTATUS_SID, MCU_E_UNINIT);
#endif
        return MCU_PLL_STATUS_UNDEFINED;
    }

    if ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
        return MCU_PLL_UNLOCKED;
    }
    
    return MCU_PLL_LOCKED;
}

#if (MCU_NO_PLL == STD_OFF)
Std_ReturnType Mcu_DistributePllClock(void) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_DISTRIBUTEPLLCLOCK_SID, MCU_E_UNINIT);
#endif
        return E_NOT_OK;
    }

    if (Mcu_GetPllStatus() != MCU_PLL_LOCKED) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_DISTRIBUTEPLLCLOCK_SID, MCU_E_PLL_NOT_LOCKED);
#endif
        return E_NOT_OK;
    }

    RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL);

    return Mcu_WaitEqual(&RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL, CLOCK_SOURCE_SWITCH_TIMEOUT);
}
#endif

Mcu_RawResetType Mcu_GetResetRawValue(void) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETRESETRAWVALUE_SID, MCU_E_UNINIT);
#endif
        return MCU_RESET_RAW_UNDEFINED;
    }

    const uint32 reset_mask = RCC_CSR_BORRSTF  | RCC_CSR_PINRSTF  | 
                              RCC_CSR_PORRSTF  | RCC_CSR_SFTRSTF  |
                              RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF |
                              RCC_CSR_LPWRRSTF;

    return (Mcu_RawResetType)(Mcu_ResetReason & reset_mask);
}

Mcu_ResetType Mcu_GetResetReason(void) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETRESETREASON_SID, MCU_E_UNINIT);
#endif
        return MCU_RESET_UNDEFINED;
    }

    if ((Mcu_ResetReason & (RCC_CSR_PORRSTF | RCC_CSR_BORRSTF)) != 0U) {
        return MCU_POWER_ON_RESET;
    }

    if ((Mcu_ResetReason & (RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF)) != 0U) {
        return MCU_WATCHDOG_RESET;
    }

    if ((Mcu_ResetReason & RCC_CSR_SFTRSTF) != 0U) {
        return MCU_SW_RESET;
    }

    if ((Mcu_ResetReason & RCC_CSR_PINRSTF) != 0U) {
        return MCU_RESET_UNDEFINED;
    }

    return MCU_RESET_UNDEFINED;
}

Std_ReturnType Mcu_InitRamSection(Mcu_RamSectionType RamSection) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON) 
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITRAMSECTION_SID, MCU_E_UNINIT);
#endif
        return E_NOT_OK;
    }

    if (RamSection >= Mcu_ConfigPtr->NumRamSectorSettings) {
#if (MCU_DEV_ERROR_DETECT == STD_ON) 
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITRAMSECTION_SID, MCU_E_PARAM_RAMSECTION);
#endif
        return E_NOT_OK;
    }

    const Mcu_RamSectorSettingConfigType* sec = &Mcu_ConfigPtr->RamSectorSettings[RamSection];
    uint32 pattern = 0;
    uint32 blocks = sec->RamSizeBytes / sec->WriteSize;
    (void)memset(&pattern, (int)sec->DefaultValue, sizeof(pattern));

    for (uint32 index = 0U; index < blocks; index++) {
        switch (sec->WriteSize) {
            case 1U: ((volatile uint8*)sec->RamBaseAddress)[index] = (uint8)pattern; break;
            case 2U: ((volatile uint16*)sec->RamBaseAddress)[index] = (uint16)pattern; break;
            case 4U: ((volatile uint32*)sec->RamBaseAddress)[index] = (uint32)pattern; break;
            default: return E_NOT_OK;
        }
    }
    
    return E_OK;
}

#if (MCU_PERFORM_RESET_API == STD_ON)
void Mcu_PerformReset(void) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_PERFORMRESET_SID, MCU_E_UNINIT);
#endif
        return;
    }

    NVIC_SystemReset();
}
#endif

void Mcu_SetMode(Mcu_ModeType McuMode) {
    if (Mcu_Status == MCU_UNINIT) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_SETMODE_SID, MCU_E_UNINIT);
#endif
        return;
    }

    const Mcu_ModeSettingConfigType* cfg = NULL_PTR;
    for (uint8 index = 0U; index < Mcu_ConfigPtr->NumModeSettings; index++) {
        if (Mcu_ConfigPtr->ModeSettings[index].ModeId == McuMode) {
            cfg = &Mcu_ConfigPtr->ModeSettings[index];
            break;
        }
    }

    if (cfg == NULL_PTR) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_SETMODE_SID, MCU_E_PARAM_MODE);
#endif
        return;
    }

    const uint32 lp_bits = PWR_CR_PDDS | PWR_CR_LPDS | PWR_CR_FPDS;
    switch (cfg->PowerMode) {
        case MCU_RUN:
            PWR->CR &= ~lp_bits;
            SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
            return;
        case MCU_SLEEP:
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            break;
        case MCU_STOP:
            PWR->CR = (PWR->CR & ~lp_bits) | (cfg->PwrControlMask & (PWR_CR_LPDS | PWR_CR_FPDS));
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            break;
        case MCU_STANDBY:
            PWR->CR |= PWR_CR_CWUF;
            PWR->CR = (PWR->CR & ~lp_bits) | PWR_CR_PDDS;
            SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
            break;
        default:
#if (MCU_DEV_ERROR_DETECT == STD_ON)
            (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_SETMODE_SID, MCU_E_PARAM_MODE);
#endif
            return;
    }

    if (cfg->SleepOnExit == TRUE) {
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    } else {
        SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
    }

    __DSB();
    __WFI();
    __ISB();

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

#if (MCU_VERSION_INFO_API == STD_ON)
void Mcu_GetVersionInfo(Std_VersionInfoType* versioninfo) {
    if (versioninfo == NULL_PTR) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETVERSIONINFO_SID, MCU_E_PARAM_POINTER);
#endif
        return;
    }

    versioninfo->vendorID = MCU_VENDOR_ID;
    versioninfo->moduleID = MCU_MODULE_ID;
    versioninfo->sw_major_version = MCU_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = MCU_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = MCU_SW_PATCH_VERSION;
}
#endif