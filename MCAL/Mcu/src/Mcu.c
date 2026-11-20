#include "Mcu.h"
#include "Dem.h"
#include "Dem_Cfg.h"
#include "Det.h"
#include "Std_Types.h"
#include "stm32f407xx.h"
#include "Mcu_Cfg.h"

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
        case 1U:
            *code = RCC_CFGR_HPRE_DIV1;
            break;
        case 2U:
            *code = RCC_CFGR_HPRE_DIV2;  
            break; 
        case 4U:
            *code = RCC_CFGR_HPRE_DIV4;
            break;
        case 8U:
            *code = RCC_CFGR_HPRE_DIV8;
            break;   
        case 16U:
            *code = RCC_CFGR_HPRE_DIV16;
            break;
        case 64U:
            *code = RCC_CFGR_HPRE_DIV64;  
            break; 
        case 128U:
            *code = RCC_CFGR_HPRE_DIV128;
            break;
        case 256U:
            *code = RCC_CFGR_HPRE_DIV256;
            break;
        case 512U:
            *code = RCC_CFGR_HPRE_DIV512;
            break;
        default:
            return E_NOT_OK;   
    }
    return E_OK;
}

static Std_ReturnType Mcu_EncodePpre1(uint32 divider, uint32* code) {
    switch (divider) {
        case 1U:
            *code = RCC_CFGR_PPRE1_DIV1;
            break;
        case 2U:
            *code = RCC_CFGR_PPRE1_DIV2;  
            break; 
        case 4U:
            *code = RCC_CFGR_PPRE1_DIV4;
            break;
        case 8U:
            *code = RCC_CFGR_PPRE1_DIV8;
            break;   
        case 16U:
            *code = RCC_CFGR_PPRE1_DIV16;
            break;
        default:
            return E_NOT_OK;
    }
    return E_OK;
}

static Std_ReturnType Mcu_EncodePpre2(uint32 divider, uint32* code) {
    switch (divider) {
        case 1U:
            *code = RCC_CFGR_PPRE2_DIV1;
            break;
        case 2U:
            *code = RCC_CFGR_PPRE2_DIV2;  
            break; 
        case 4U:
            *code = RCC_CFGR_PPRE2_DIV4;
            break;
        case 8U:
            *code = RCC_CFGR_PPRE2_DIV8;
            break;   
        case 16U:
            *code = RCC_CFGR_PPRE2_DIV16;
            break;
        default:
            return E_NOT_OK;
    }
    return E_OK;
}


void Mcu_Init(const Mcu_ConfigType* ConfigPtr) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (ConfigPtr == NULL_PTR) {
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INIT_SID, MCU_E_PARAM_POINTER);
        return;
    }
#endif

    Mcu_ResetReason = RCC->CSR;
    RCC->CSR |= RCC_CSR_RMVF;

    Mcu_ConfigPtr = ConfigPtr;
    Mcu_Status    = MCU_INITIALIZED;
}

Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting) {
    Mcu_ClockStarted = FALSE;
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (Mcu_Status != MCU_INITIALIZED) {
        Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_INITCLOCK_SID, MCU_E_PARAM_UNINIT);
        return E_NOT_OK;
    }
#endif

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

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    (void)RCC->APB1ENR;

    PWR->CR |= PWR_CR_VOS;

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

Mcu_PllStatusType Mcu_GetPllStatus(void) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (Mcu_Status == MCU_UNINIT) {
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETPLLSTATUS_SID, MCU_E_PARAM_UNINIT);
        return MCU_PLL_STATUS_UNDEFINED;
    }
#endif

    if ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
        return MCU_PLL_UNLOCKED;
    }
    
    return MCU_PLL_LOCKED;
}

Std_ReturnType Mcu_DistributePllClock(void) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (Mcu_Status == MCU_UNINIT) {
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_DISTRIBUTEPLLCLOCK_SID, MCU_E_PARAM_UNINIT);
        return E_NOT_OK;
    }

    if (Mcu_GetPllStatus() != MCU_PLL_LOCKED) {
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_DISTRIBUTEPLLCLOCK_SID, MCU_E_PLL_NOT_LOCKED);
        return E_NOT_OK;
    }
#endif

    RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL);

    return Mcu_WaitEqual(&RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL, CLOCK_SOURCE_SWITCH_TIMEOUT);
}

Mcu_RawResetType Mcu_GetResetRawValue(void) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (Mcu_Status == MCU_UNINIT) {
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETRESETRAWVALUE_SID, MCU_E_PARAM_UNINIT);
        return MCU_RESET_RAW_UNDEFINED;
    }
#endif

    const uint32 reset_mask = RCC_CSR_BORRSTF  | RCC_CSR_PINRSTF  | 
                              RCC_CSR_PORRSTF  | RCC_CSR_SFTRSTF  |
                              RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF |
                              RCC_CSR_LPWRRSTF;

    return (Mcu_RawResetType)(Mcu_ResetReason & reset_mask);
}

Mcu_ResetType Mcu_GetResetReason(void) {
#if (MCU_DEV_ERROR_DETECT == STD_ON)
    if (Mcu_Status == MCU_UNINIT) {
        (void)Det_ReportError(MCU_MODULE_ID, MCU_INSTANCE_ID, MCU_GETRESETREASON_SID, MCU_E_PARAM_UNINIT);
        return MCU_UNDEFINED_RESET;
    }
#endif

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
        return MCU_UNDEFINED_RESET;
    }

    return MCU_UNDEFINED_RESET;
}