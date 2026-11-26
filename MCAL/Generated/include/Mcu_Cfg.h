#ifndef MCU_CFG_H
#define MCU_CFG_H

#include "Std_Types.h"

#define MCU_MODULE_ID               ((uint16)101)
#define MCU_INSTANCE_ID             ((uint8)0)

/* ============================================================================
 * Configuration
 * ========================================================================== */

#define MCU_VARIANT_POST_BUILD              STD_ON

#define MCU_DEV_ERROR_DETECT                STD_ON

#define MCU_INIT_CLOCK                      STD_ON
#define MCU_NO_PLL                          STD_OFF
#define MCU_PERFORM_RESET_API               STD_ON
#define MCU_VERSION_INFO_API                STD_ON
#define MCU_GET_RAM_STATE_API               STD_OFF


/* ============================================================================
 * Oscillator frequencies
 *
 * Generated from ECU configuration.
 * ========================================================================== */

#define MCU_PLL_SOURCE_HSI                 ((uint32)0)
#define MCU_PLL_SOURCE_HSE                 ((uint32)1)

#define MCU_HSE_FREQUENCY_HZ               (8000000UL)
#define MCU_HSI_FREQUENCY_HZ               (16000000UL)


/* ============================================================================
 * Timeout configuration
 * ========================================================================== */

#define MCU_HSI_START_TIMEOUT              (1000000UL)
#define MCU_HSE_START_TIMEOUT              (1000000UL)
#define MCU_PLL_STOP_TIMEOUT               (1000000UL)
#define MCU_SYSCLK_SWITCH_TIMEOUT          (1000000UL)
#define MCU_VOS_READY_TIMEOUT              (1000000UL)


/* ============================================================================
 * Generated clock IDs
 * ========================================================================== */

#define McuConf_McuClockSettingConfig_Clock_168MHz \
    ((Mcu_ClockType)0U)

/* ============================================================================
 * Generated mode IDs
 * ========================================================================== */

#define McuConf_McuModeSettingConfig_Sleep \
    ((Mcu_ModeType)0U)

#define McuConf_McuModeSettingConfig_Stop \
    ((Mcu_ModeType)1U)

#define McuConf_McuModeSettingConfig_Standby \
    ((Mcu_ModeType)2U)

/* ============================================================================
 * Configuration version
 * ========================================================================== */

#define MCU_CFG_MAJOR_VERSION              1U
#define MCU_CFG_MINOR_VERSION              0U
#define MCU_CFG_PATCH_VERSION              0U

#endif // MCU_CFG_H