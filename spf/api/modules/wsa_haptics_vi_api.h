#ifndef __WSA_HAPTICS_VI_API_H__
#define __WSA_HAPTICS_VI_API_H__

/*==============================================================================
 *  Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 *  SPDX-License-Identifier: BSD-3-Clause-Clear

  @file wsa_haptics_vi_api.h
  @brief This file contains APIs for Haptics Module
==============================================================================*/

/*==============================================================================
  Edit History

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
 06/21/2022  peri       Haptics_VI
==============================================================================*/

/** @h2xml_title1           {Haptics}
    @h2xml_title_date       {June 21, 2022} */
#include "module_cmn_api.h"
#include "ar_defs.h"
#include "imcl_spm_intent_api.h"
#include "rtm_logging_api.h"


/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/
/*==============================================================================
   Constants
==============================================================================*/
#define HAPTICS_VI_MAX_INPUT_PORTS 1
#define HAPTICS_VI_MAX_OUTPUT_PORTS 1
#define HAPTICS_VI_STACK_SIZE 4096

/* Unique Module ID */
#define MODULE_ID_HAPTICS_VI 0x070010E5

/** @h2xmlm_module       {"MODULE_ID_HAPTICS_VI",
                          MODULE_ID_HAPTICS_VI}
    @h2xmlm_displayName  {"HAPTICS - VI"}
    @h2xmlm_toolPolicy   {Calibration;CALIBRATION}
    @h2xmlm_description  {ID of the HAPTICS VI module.\n
      this module includes VI Sens feedback data processing,
      Thermal tracking, LRA state tracking.

      This module supports the following parameter IDs:\n
     - #PARAM_ID_HAPTICS_VI_STATIC_CONFIG\n
     - #PARAM_ID_HAPTICS_VI_OP_MODE_PARAM\n
     - #PARAM_ID_HAPTICS_VI_EX_FTM_MODE_PARAM\n
     - #PARAM_ID_HAPTICS_TH_VI_R0T0_SET_PARAM\n
     - #PARAM_ID_HAPTICS_TH_VI_R0_GET_PARAM\n
     - #PARAM_ID_HAPTICS_TH_VI_DYNAMIC_PARAM\n
     - #PARAM_ID_HAPTICS_TH_VI_FTM_SET_CFG\n
     - #PARAM_ID_HAPTICS_TH_VI_FTM_GET_PARAM\n
     - #PARAM_ID_HAPTICS_EX_VI_DYNAMIC_PARAM\n
     - #PARAM_ID_HAPTICS_EX_VI_PERSISTENT\n
     - #PARAM_ID_HAPTICS_EX_VI_FTM_SET_CFG\n
     - #PARAM_ID_HAPTICS_EX_VI_FTM_GET\n
     - #PARAM_ID_HAPTICS_LRA_DIAG_GETPKT_PARAM\n
     - #PARAM_ID_HAPTICS_EX_VI_DEMO_PKT_PARAM\n
     - #PARAM_ID_HAPTICS_VI_CHANNEL_MAP_CFG\n

      All parameter IDs are device independent.\n

     Supported Input Media Format:\n
*  - Data Format          : FIXED_POINT\n
*  - fmt_id               : Don't care\n
*  - Sample Rates         : 48000\n
*  - Number of channels   : 2, 4\n
*  - Channel type         : 1 to 63\n
*  - Bits per sample      : 16 , 32 \n
*  - Q format             : 15 for bps = 16 and 27 for bps = 32\n
*  - Interleaving         : de-interleaved unpacked\n
*  - Signed/unsigned      : Signed }

     @h2xmlm_toolPolicy              {Calibration}

    @h2xmlm_dataMaxInputPorts    {HAPTICS_VI_MAX_INPUT_PORTS}
     @h2xmlm_dataInputPorts       {IN=2}
     @h2xmlm_dataMaxOutputPorts   {HAPTICS_VI_MAX_OUTPUT_PORTS}
     @h2xmlm_dataOutputPorts      {OUT=1}
     @h2xmlm_supportedContTypes  {APM_CONTAINER_TYPE_GC}
     @h2xmlm_isOffloadable       {false}
     @h2xmlm_stackSize            {HAPTICS_VI_STACK_SIZE}
    @h2xmlm_ctrlDynamicPortIntent  { "HAPTICS VI intent id for communicating Vsens and Isens data" = INTENT_ID_HAPTICS, maxPorts=
1 }
     @h2xmlm_ToolPolicy              {Calibration}

    @{                   <-- Start of the Module -->

 @h2xml_Select        {"param_id_module_enable_t"}
 @h2xmlm_InsertParameter

 @h2xml_Select        {"rtm_logging_enable_payload_t"}
 @h2xmlm_InsertParameter
   */

/*------------------------------------------------------------------------------
   Parameters
------------------------------------------------------------------------------*/

/*==============================================================================
   Constants
==============================================================================*/
/* Vsens from the left speaker. */
#define HAPTICS_VI_VSENS_LEFT_CHAN 1

/* Isens from the left speaker. */
#define HAPTICS_VI_ISENS_LEFT_CHAN 2

/* Vsens from the right speaker. */
#define HAPTICS_VI_VSENS_RIGHT_CHAN 3

/* Isens from the right speaker. */
#define HAPTICS_VI_ISENS_RIGHT_CHAN 4

/*==============================================================================
   Constants
==============================================================================*/

// Maximum output channels
#define HAPTICS_MAX_OUT_CHAN  (2)

// Maximum number of output channels
//#define HAPTICS_MAX_OUT_CHAN  (HAPTICS_MAX_OUT_CHAN)

#define MAX_SAMPLES_IN_PACKET 480

/*typedef enum haptics_th_vi_cali_state_t
{
    HAPTICS_TH_VI_CALI_STATE_INCORRECT_OP_MODE = 0, // returned if "operation_mode" is not "Thermal Calibration"
    HAPTICS_TH_VI_CALI_STATE_INACTIVE = 1, // init value
    HAPTICS_TH_VI_CALI_STATE_WARMUP = 2, // wait state for warmup
    HAPTICS_TH_VI_CALI_STATE_INPROGRESS = 3, // in calibration state
    HAPTICS_TH_VI_CALI_STATE_SUCCESS = 4, // calibration successful
    HAPTICS_TH_VI_CALI_STATE_FAILED = 5, // calibration failed
    HAPTICS_TH_VI_CALI_STATE_WAIT_FOR_VI = 6, // wait state for vi threshold
    HAPTICS_TH_VI_CALI_STATE_VI_WAIT_TIMED_OUT = 7, // calibration could not start due to vi signals below threshold
    HAPTICS_TH_VI_CALI_STATE_LOW_VI = 8, // calibration failed due to vi signals below threshold
    HAPTICS_TH_VI_CALI_STATE_MAX_VAL = 0xFFFFFFFF // // max 32-bit unsigned value. tells the compiler to use 32-bit data type
} haptics_th_vi_cali_state_t;

// FTM support
typedef enum haptics_th_vi_ftm_state_t
{
   HAPTICS_TH_VI_FTM_STATE_INCORRECT_OP_MODE = 0, // returned if "operation_mode" is not "Thermal FTM"
   HAPTICS_TH_VI_FTM_STATE_INACTIVE          = 1, // init value
   HAPTICS_TH_VI_FTM_STATE_WARMUP            = 2, // wait state
   HAPTICS_TH_VI_FTM_STATE_INPROGRESS        = 3, // in FTM state
   HAPTICS_TH_VI_FTM_STATE_SUCCESS           = 4, // FTM successful
   HAPTICS_TH_VI_FTM_STATE_FAILED            = 5, // FTM failed
   HAPTICS_TH_VI_FTM_STATE_MAX_VAL = 0xFFFFFFFF // // max 32-bit unsigned value. tells the compiler to use 32-bit data type
} haptics_th_vi_ftm_state_t;

// FTM support
typedef enum haptics_ex_vi_ftm_state_t
{
   HAPTICS_EX_VI_FTM_STATE_INCORRECT_OP_MODE = 0, // returned if "operation_mode" is not "FTM"
   HAPTICS_EX_VI_FTM_STATE_INACTIVE          = 1, // init value
   HAPTICS_EX_VI_FTM_STATE_WARMUP            = 2, // wait state
   HAPTICS_EX_VI_FTM_STATE_INPROGRESS        = 3, // in FTM state
   HAPTICS_EX_VI_FTM_STATE_SUCCESS           = 4, // FTM successful
   HAPTICS_EX_VI_FTM_STATE_FAILED            = 5, // FTM failed
   HAPTICS_EX_VI_FTM_STATE_MAX_VAL = 0xFFFFFFFF // // max 32-bit unsigned value. tells the compiler to use 32-bit data type
} haptics_ex_vi_ftm_state_t;

// LRA diagnostics states in normal operation mode
typedef enum haptics_th_vi_lra_cond_t
{
    HAPTICS_TH_VI_LRA_COND_OK = 0,
    HAPTICS_TH_VI_LRA_COND_SHORT = 1,
    HAPTICS_TH_VI_LRA_COND_OPEN = 2,
    HAPTICS_TH_VI_LRA_COND_DC = 3,
    HAPTICS_TH_VI_LRA_COND_MAX_VAL = 0xFFFFFFFF // // max 32-bit unsigned value. tells the compiler to use 32-bit data type
} haptics_th_vi_lra_cond_t;

// LRA temperature status in normal operation mode
typedef enum haptics_th_vi_lra_temperature_t
{
    HAPTICS_TEMPERATURE_OK = 0,
    HAPTICS_TEMPERATURE_ABOVE_MAX_LIMIT = 1,
    HAPTICS_TEMPERATURE_BELOW_MIN_LIMIT = 2,
    HAPTICS_TEMPERATURE_MAX_VAL = 0xFFFFFFFF // // max 32-bit unsigned value. tells the compiler to use 32-bit data type
} haptics_th_vi_lra_temperature_t;*/

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_VI_STATIC_CONFIG 0x080013A4

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_vi_static_config_t param_id_haptics_vi_static_config_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_VI_STATIC_CONFIG",
                         PARAM_ID_HAPTICS_VI_STATIC_CONFIG}
    @h2xmlp_description {Parameter used to configure static parameters of VI processing.}
    @h2xmlp_toolPolicy  {CALIBRATION;RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_vi_static_config_t {
   uint32_t num_channels;
   /**< @h2xmle_description {Number of channels for Rx signal.}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
   uint32_t sampling_rate;
   /**< @h2xmle_description {Sampling rate of VI signal.}
        @h2xmle_rangeList   {"48kHz"=48000}
        @h2xmle_default     {48000} */
   uint32_t fpilot_Hz;
   /**< @h2xmle_description {Pilot tone frequency, in Hz. Pilot tone frequency values MUST be the same for the RX/TH/EX modules}
        @h2xmle_rangeList   {"40"=40}
        @h2xmle_default     {40} */
   int32_t fpilot_amplitude_db;    // Pilot tone amplitude [-37] -37dB.
                                  // Note: Pilot tone amplitude values MUST be the same for the RX/TH/EX modules.
   /**< @h2xmle_description {Pilot tone amplitude relative to 0 dbFS (for example, -37 dB from the full scale), in dB. Pilot tone amplitude values MUST be the same for the RX/TH/EX modules}
        @h2xmle_range       {-40..-30} dB
        @h2xmle_default     {-37} */
    int32_t smoothing_time_const_q24;
    /**< @h2xmle_description {Smoothing time constant for f0 tracking parameter update. Default = 0.1, Range: 0 to 1}
         @h2xmle_range       {0..16777216}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {1677721} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;


/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_VI_OP_MODE_PARAM 0x080013A5

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_vi_op_mode_param_t param_id_haptics_vi_op_mode_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_VI_OP_MODE_PARAM",
                          PARAM_ID_HAPTICS_VI_OP_MODE_PARAM}
    @h2xmlp_description {Parameter used to select the operation mode of WSA smart haptics VI processing.}
    @h2xmlp_toolPolicy  {NO_SUPPORT}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_vi_op_mode_param_t {
   uint32_t th_operation_mode;   // normal running or calibration
   /**< @h2xmle_description {Operation mode of thermal VI module.}
        @h2xmle_rangeList   {"Normal Running mode"=0;
                             "Calibration mode"=1;
                             "Factory Test Mode"=2} */

   uint32_t th_r0t0_selection_flag[HAPTICS_MAX_OUT_CHAN]; // this flag determines which set of R0, T0 values the algorithm will use
                                                          // (0: use calibrated R0, T0 value, 1: use safe R0, T0 value)
    /**< @h2xmle_description {Specifies which set of R0, T0 values the algorithm will use. This field is valid only in
                              Normal mode (operation_mode = 0).}
         @h2xmle_rangeList   {"Use calibrated R0, T0 value"=0;"Use safe R0, T0 value"=1} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_VI_EX_FTM_MODE_PARAM 0x080013A6

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_vi_ex_FTM_mode_param_t param_id_haptics_vi_ex_FTM_mode_param_t;

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_VI_EX_FTM_MODE_PARAM",
                          PARAM_ID_HAPTICS_VI_EX_FTM_MODE_PARAM}
    */

struct param_id_haptics_vi_ex_FTM_mode_param_t {
  uint32_t ex_FTM_mode_enable_flag;
   /**< @h2xmle_description {Factory Test Mode enablement flag. When enabled, the estimated lra parameters are averaged
                             and pushed out to HAL for parameter display and saving into audio.cal}
        @h2xmle_rangeList        {"disabled"=0; "enabled"=1}
        @h2xmle_default          {0} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_TH_VI_R0T0_SET_PARAM 0x080013A7

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_th_vi_r0t0_set_param_t param_id_haptics_th_vi_r0t0_set_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_TH_VI_R0T0_SET_PARAM",
                          PARAM_ID_HAPTICS_TH_VI_R0T0_SET_PARAM}
    @h2xmlp_description {Parameter used to configure static R0T0 parameters of thermal VI processing}
    @h2xmlp_toolPolicy  {NO_SUPPORT}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_th_vi_r0t0_set_param_t {
   uint32_t num_channels;
    /**< @h2xmle_description {Number of channels for Rx signal.}
         @h2xmle_rangeList   {"1"=1;"2"=2}
         @h2xmle_default     {1} */
    int32_t r0_cali_q24[HAPTICS_MAX_OUT_CHAN];           // calibration point resistance (in ohm), per device
    /**< @h2xmle_description {Calibration point resistance per device (in Ohms).  This field is valid only  in Normal mode (operation_mode = 0). Default = 8Ohms, Range: 2 to 64Ohms}
         @h2xmle_range       {33554432..1073741824}
         @h2xmle_default     {134217728}
         @h2xmle_dataFormat  {Q24} */
    int32_t t0_cali_q6[HAPTICS_MAX_OUT_CHAN];            // caliberation point Temp (deg C), per device
    /**< @h2xmle_description {Calibration point temperature per device (in degrees C). This field is valid in both Normal mode and Calibration mode. Default = 25 deg C, Range: -30 to 80 deg C.}
         @h2xmle_range       {-1920..5120}
         @h2xmle_dataFormat  {Q6}
         @h2xmle_default     {1600} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;


/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_TH_VI_R0_GET_PARAM 0x080013A8

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_th_vi_r0_get_param_t param_id_haptics_th_vi_r0_get_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_TH_VI_R0_GET_PARAM",
                          PARAM_ID_HAPTICS_TH_VI_R0_GET_PARAM}
    */

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_th_vi_r0_get_param_t {
  uint32_t state; // calibration state
   /**< @h2xmle_description {Calibration state}
        @h2xmle_range       {0...8} */
   int32_t r0_cali_q24[HAPTICS_MAX_OUT_CHAN]; // calibration point resistance (in ohm), per device
   /**< @h2xmle_description {Calibration point resistance per device (in Ohms). Default = 8Ohms, Range: 2 to 64Ohms}
         @h2xmle_range       {33554432..1073741824}
         @h2xmle_default     {134217728}
         @h2xmle_dataFormat  {Q24} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_TH_VI_DYNAMIC_PARAM 0x080013A9

/*==============================================================================
   Type definitions
==============================================================================*/

// Structure encapsulating LRA parameters for Thermal VI module
typedef struct wsa_haptics_th_lra_param_t wsa_haptics_th_lra_param_t;

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
/** @h2xmlp_subStruct */

struct wsa_haptics_th_lra_param_t
{
    int32_t r0_safe_q24;    // backup calibration point resistance (ohm), when per device calibration is not available
    /**< @h2xmle_description {backup calibration point resistance when per-device calibration is not available (in
                              Ohms.) Default = 8Ohms, Range: 2 to 64Ohms}
         @h2xmle_range       {33554432..1073741824}
         @h2xmle_default     {134217728}
         @h2xmle_dataFormat  {Q24} */
    int32_t t0_safe_q6; // backup caliberation point Temp (deg C), when per device calibration is not available
    /**< @h2xmle_description {backup calibration point temperature when per-device calibration is not available (in
                              degrees C). Default = 25 deg C, Range: -30 to 80 deg C.}
         @h2xmle_range       {-1920..5120}
         @h2xmle_dataFormat  {Q6}
         @h2xmle_default     {1600} */
    int16_t amp_gain_q10;   // LRA peak voltage for digitially full scale signal
    /**< @h2xmle_description {voltage for digitally full scale signal. Default = 10, Range: 0 to 32}
         @h2xmle_range       {0..32767}
         @h2xmle_dataFormat  {Q10}
         @h2xmle_default     {10240} */
    uint16_t lra_resistivity_coeff_u16q7;   // resistivity coeff (1/deg C)
    /**< @h2xmle_description {one over temperature coeffcient alpha in 1/deg C. Default = 254.45, Range: 0 to 511.99}
         @h2xmle_range       {0..65535}
         @h2xmle_dataFormat  {Q7}
         @h2xmle_default     {32570} */
    uint16_t vsen_2_v_u16q11;   // conversion factor from Vsen digital to Volt
    /**< @h2xmle_description {conversion factor from Vsens digital to Volts. Default = 10.48, Range: 0 to 31.99}
         @h2xmle_range       {0..65535}
         @h2xmle_dataFormat  {Q11}
         @h2xmle_default     {21466} */
    uint16_t isen_2_a_u16q11;   // conversion factor from Isen digital to Amp
    /**< @h2xmle_description {conversion factor from Isens digital to Amps. Default = 6.90, Range: 0 to 31.99}
         @h2xmle_range       {0..65535}
         @h2xmle_dataFormat  {Q11}
         @h2xmle_default     {14143} */
    int32_t trace_resistance_tx_q24;
    /**< @h2xmle_description {trace resistance from LRA to Vsens tap points in Ohm. Default = 0, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t Re_ohm_q24;
    /**< @h2xmle_description {DC resistance of LRA coil at room temperature, in Ohms. Default = 8Ohms, Range: 2 to 128Ohms}
         @h2xmle_range       {33554432..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {134217728} */
    int32_t r_lra_coil_factor_q10;
    /**< @h2xmle_description {factor used to determine the threshold
                              for LRA coil resistance (max threshold for LRA
                              coil resistance = factor * LRA coil resistance and min threshold = LRA coil resistance/ factor). Default = 2, Range: 1 to 8}
         @h2xmle_range       {1024..8192}
         @h2xmle_dataFormat  {Q10}
         @h2xmle_default     {2048} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/* Structure encapsulating LRA parameters for R0T0 calibration*/
typedef struct wsa_haptics_r0t0_cali_param_t wsa_haptics_r0t0_cali_param_t;

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
/** @h2xmlp_subStruct */

struct wsa_haptics_r0t0_cali_param_t
{
    int32_t wait_time_ms;   // wait time for initial settling before collecting statistic (ms)
    /**< @h2xmle_description {wait time for initial settling before
         collecting statistics (in milli seconds).}
         @h2xmle_range       {100..5000}
         @h2xmle_default     {200} */
    int32_t cali_time_ms;   // calibration time (ms)
    /**< @h2xmle_description {calibration time  (in milli seconds).}
         @h2xmle_range       {500..30000}
         @h2xmle_default     {2000} */
    int32_t vi_wait_time_ms;                 // wait time to check if VI are above threshold levels
    /**< @h2xmle_description {wait time for vi (in milli seconds).}
         @h2xmle_range       {100..30000}
         @h2xmle_default     {2000} */
    int16_t r0_rng_fac_q10;
    /**< @h2xmle_description {If the calibrated R0 is greater than range factor times the typical value, or less than the      typical value divided by the range factor, calibration failed will be called. Default = 1.5, Range: 1 to 8}
         @h2xmle_range       {1024..8192}
         @h2xmle_dataFormat  {Q10}
         @h2xmle_default     {1536} */
    int16_t t0_min_q6;  // min expected temperature from calibration (deg C)
    /**< @h2xmle_description {minimum expected temperature from
                              calibration (in degrees C). Default = 0 deg C, Range: -30 to 80 deg C}
         @h2xmle_range       {-1920..5120}
         @h2xmle_dataFormat  {Q6}
         @h2xmle_default     {0} */
    int16_t t0_max_q6;  // max expected temperature from calibration (deg C)
    /**< @h2xmle_description {maximum expected temperature from
                              calibration (in degrees C). Default = 50 deg C, Range: -30 to 80 deg C}
         @h2xmle_range       {-1920..5120}
         @h2xmle_dataFormat  {Q6}
         @h2xmle_default     {3200} */
    uint16_t reserved;
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/* Structure definition for Parameter */
typedef struct param_id_haptics_th_vi_dynamic_param_t param_id_haptics_th_vi_dynamic_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_TH_VI_DYNAMIC_PARAM",
                          PARAM_ID_HAPTICS_TH_VI_DYNAMIC_PARAM}
    @h2xmlp_description {Parameter used to configure dynamic thermal parameters for WSA smart haptics VI processing.}
    @h2xmlp_toolPolicy  {CALIBRATION;RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_th_vi_dynamic_param_t {
    uint32_t num_channels;
   /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
   int32_t temp_min_q6;
   /**< @h2xmle_description {Minimum reasonable measured temperature (in degree C). Default = -30 deg C, Range: -30 to 200 deg C}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {-1920} */
   int32_t temp_max_q6;
   /**< @h2xmle_description {Maximum reasonable measured temperature (in degree C). Default = 200 deg C, Range: -30 to 80 deg C}
       @h2xmle_range       {-1920..12800}
       @h2xmle_dataFormat  {Q6}
       @h2xmle_default     {12800} */
#ifdef __H2XML__
   wsa_haptics_th_lra_param_t wsa_haptics_th_lra_param[0];
   /**< @h2xmle_description {Structure encapsulating LRA parameters for Thermal VI module.}
        @h2xmle_variableArraySize  {num_channels} */
   wsa_haptics_r0t0_cali_param_t wsa_haptics_r0t0_cali_param[0];
   /**< @h2xmle_description {Structure encapsulating LRA parameters for R0T0 calibration.}
        @h2xmle_variableArraySize  {num_channels} */
#endif
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_TH_VI_FTM_SET_CFG 0x080013AA

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_th_vi_ftm_set_cfg param_id_haptics_th_vi_ftm_set_cfg;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_TH_VI_FTM_SET_CFG",
                         PARAM_ID_HAPTICS_TH_VI_FTM_SET_CFG}
    @h2xmlp_description {Parameter used to set the FTM configuration in the WSA smart haptics thermal VI processing module.}
    @h2xmlp_toolPolicy  {CALIBRATION;RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
struct param_id_haptics_th_vi_ftm_set_cfg {
  uint32_t num_channels;
   /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
   int32_t wait_time_ms[HAPTICS_MAX_OUT_CHAN]; // wait time before collecting statistics for ftm mode (ms)
   /**< @h2xmle_description {wait time before collecting statistics for ftm mode (ms)}
        @h2xmle_range       {100..5000}
        @h2xmle_dataFormat  {Q0}
        @h2xmle_default     {2000} */
   int32_t ftm_time_ms[HAPTICS_MAX_OUT_CHAN];  // duration that ftm packets will be averaged
   /**< @h2xmle_description {duration that ftm packets will be averaged, in ms}
        @h2xmle_range       {100..5000}
        @h2xmle_dataFormat  {Q0}
        @h2xmle_default     {2000} */
}
#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_TH_VI_FTM_GET_PARAM 0x080013AB

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_th_vi_ftm_get_param_t param_id_haptics_th_vi_ftm_get_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_TH_VI_FTM_GET_PARAM",
                          PARAM_ID_HAPTICS_TH_VI_FTM_GET_PARAM}
    @h2xmlp_description {Thermal VI FTM mode parameters returned from WSA Smart haptics thermal FTM mode}
    @h2xmlp_toolPolicy  {RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
struct param_id_haptics_th_vi_ftm_get_param_t {
   uint32_t num_channels;
   /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
   int32_t                ftm_rDC_q24[HAPTICS_MAX_OUT_CHAN];  // rDC value in q24
   /**< @h2xmle_description {resistance in ohm}
        @h2xmle_range       {0..2147483647}
        @h2xmle_dataFormat  {Q24}
        @h2xmle_default     {0} */
   int32_t                ftm_temp_q22[HAPTICS_MAX_OUT_CHAN]; // temperature value in q22
   /**< @h2xmle_description {temperature in degree C}
        @h2xmle_range       {0..2147483647}
        @h2xmle_dataFormat  {Q22}
        @h2xmle_default     {0} */
   uint32_t status[HAPTICS_MAX_OUT_CHAN];       // wait, in progress or success
   /**< @h2xmle_description {wait, in progress or success}
        @h2xmle_range       {0..5} */
}
#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_EX_VI_DYNAMIC_PARAM 0x080013AC

/*==============================================================================
   Type definitions
==============================================================================*/

// Structure encapsulating LRA parameters for VI excursion module
typedef struct wsa_haptics_ex_lra_param_t wsa_haptics_ex_lra_param_t;

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
/** @h2xmlp_subStruct */

struct wsa_haptics_ex_lra_param_t
{
    int32_t Re_ohm_q24;
    /**< @h2xmle_description {DC resistance of LRA coil at room temperature, in Ohms. Default = 8Ohm, Range: 2 to 128Ohms}
         @h2xmle_range       {33554432..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {134217728} */
    int32_t Le_mH_q24;
    /**< @h2xmle_description {Inductance of LRA coil at room temperature, in mH. Default = 0.04mH, Range: 0.001mH to 128mH}
         @h2xmle_range       {16777..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {671088} */
    int32_t Bl_q24;
    /**< @h2xmle_description {Force factor (Bl product). Default = 1, Range: 0.01 to 128}
         @h2xmle_range       {167772..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {16777216} */
    int32_t Mms_gram_q24;
    /**< @h2xmle_description {Mass of LRA moving mass, in grams. Default = 1g, Range: 0.05 to 128g}
         @h2xmle_range       {838860..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {16777216} */
    int32_t Rms_KgSec_q24;
    /**< @h2xmle_description {Mechanical damping or resistance of LRA, in kg/s. Default = 0.0945kg/s, Range = 0.0001 to 20kg/s}
         @h2xmle_range       {16777..335544320}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {1585446} */
    int32_t Kms_Nmm_q24;
    /**< @h2xmle_description {Mechanical stiffness of driver suspension, in N/mm. Default = 1.838N/mm, Range = 0.1 to 50N/mm}
         @h2xmle_range       {1677721..838860800}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {30836523} */
    int32_t Fres_Hz_q20;
    /**< @h2xmle_description {Resonance frequency, in Hz. Default = 160Hz, Range = 50 to 400Hz}
         @h2xmle_range       {52428800..419430400}
         @h2xmle_dataFormat  {Q20}
         @h2xmle_default     {167772160} */
    int32_t amp_gain_q24;
    /**< @h2xmle_description {LRA peak voltage for a digitally full-scale signal. Default = 10, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {167772160} */
    int32_t vsens_scale_q24; // Vsense scaling constant
    /**< @h2xmle_description {conversion factor from Vsens digital to Volts. Default = 10.48, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {175849472} */
    int32_t isens_scale_q24; // Isense scaling constant
    /**< @h2xmle_description {conversion factor from Isens digital to Amperes. Default = 6.90, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {115859456} */
    int32_t trace_resistance_tx_q24;
    /**< @h2xmle_description {trace resistance from LRA to Vsens tap points in Ohm. Default = 0Ohm, Range: 0 to 128Ohm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t trace_resistance_rx_q24;
    /**< @h2xmle_description {trace resistance from amp output to LRA in Ohm. Default = 0Ohm, Range: 0 to 128Ohm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t Min_Bl_q24;
    /**< @h2xmle_description {Force factor (Bl product) - Min limit. Default = 0.01, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {167772} */
    int32_t Max_Bl_q24;
    /**< @h2xmle_description {Force factor (Bl product) - Max limit. Default = 10, Range: 0 to 128}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {167772160} */
    int32_t Min_Rms_KgSec_q24;
    /**< @h2xmle_description {Mechanical damping or resistance of LRA in Kg/sec - Min limit. Default = 0.001Kg/sec, Range: 0 to 128Kg/sec}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {16777} */
    int32_t Max_Rms_KgSec_q24;
    /**< @h2xmle_description {Mechanical damping or resistance of LRA in Kg/sec - Max limit. Default = 20Kg/sec, Range: 0 to 128Kg/sec}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {335544320} */
    int32_t Min_Kms_Nmm_q24;
    /**< @h2xmle_description {Mechanical stiffness of driver suspension in N/mm - Min limit. Default = 0.1N/mm, Range: 0 to 128N/mm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {1677721} */
    int32_t Max_Kms_Nmm_q24;
    /**< @h2xmle_description {Mechanical stiffness of driver suspension in N/mm - Max limit. Default = 50N/mm, Range: 0 to 128N/mm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {838860800} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/* Structure definition for Parameter */
typedef struct param_id_haptics_ex_vi_dynamic_param_t param_id_haptics_ex_vi_dynamic_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_EX_VI_DYNAMIC_PARAM",
                          PARAM_ID_HAPTICS_EX_VI_DYNAMIC_PARAM}
    @h2xmlp_description {Parameter used to configure the dynamic parameters of excursion VI processing.}
    @h2xmlp_toolPolicy  {CALIBRATION;RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_ex_vi_dynamic_param_t {
   uint32_t num_channels;
   /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
#ifdef __H2XML__
   wsa_haptics_ex_lra_param_t wsa_haptics_ex_lra_param[0];
   /**< @h2xmle_description {Structure encapsulating LRA parameters for VI excursion module}
        @h2xmle_variableArraySize  {num_channels} */
#endif
}
#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_EX_VI_PERSISTENT 0x080013AD

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_ex_vi_persistent param_id_haptics_ex_vi_persistent;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_EX_VI_PERSISTENT",
                          PARAM_ID_HAPTICS_EX_VI_PERSISTENT}
    @h2xmlp_description {Excursion VI persistent parameters}
    @h2xmlp_toolPolicy  {RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_ex_vi_persistent {
    int32_t Re_ohm_q24[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {DC resistance of LRA coil at room temperature, in Ohms. Default = 8Ohm, Range: 2 to 128Ohms}
         @h2xmle_range       {33554432..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {134217728} */
    int32_t Le_mH_q24[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {Inductance of LRA coil at room temperature, in mH. Default = 0.04mH, Range: 0.001mH to 128mH}
         @h2xmle_range       {16777..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {671088} */
    int32_t Bl_q24[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {Force factor (Bl product). Default = 1, Range: 0.01 to 128}
         @h2xmle_range       {167772..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {16777216} */
    int32_t Rms_KgSec_q24[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {Mechanical damping or resistance of LRA, in kg/s. Default = 0.0945kg/s, Range = 0.0001 to 20kg/s}
         @h2xmle_range       {16777..335544320}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {1585446} */
    int32_t Kms_Nmm_q24[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {Mechanical stiffness of driver suspension, in N/mm. Default = 1.838N/mm, Range = 0.1 to 50N/mm}
         @h2xmle_range       {1677721..838860800}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {30836523} */
    int32_t Fres_Hz_q20[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {Resonance frequency, in Hz. Default = 160Hz, Range = 50 to 400Hz}
         @h2xmle_range       {52428800..419430400}
         @h2xmle_dataFormat  {Q20}
         @h2xmle_default     {167772160} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_EX_VI_FTM_SET_CFG 0x080013AE

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_ex_vi_ftm_set_cfg param_id_haptics_ex_vi_ftm_set_cfg;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_EX_VI_FTM_SET_CFG",
                          PARAM_ID_HAPTICS_EX_VI_FTM_SET_CFG}
    @h2xmlp_description {Excursion VI FTM mode parameters}
    @h2xmlp_toolPolicy  {CALIBRATION;RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_ex_vi_ftm_set_cfg {
   uint32_t num_channels;
   /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
   int32_t wait_time_ms[HAPTICS_MAX_OUT_CHAN];
   /**< @h2xmle_description {wait time before collecting statistics for ftm mode, in ms}
        @h2xmle_range       {100..5000}
        @h2xmle_dataFormat  {Q0}
        @h2xmle_default     {2000} */
   int32_t ftm_time_ms[HAPTICS_MAX_OUT_CHAN];
   /**< @h2xmle_description {duration that ftm packets will be averaged, in ms}
        @h2xmle_range       {100..5000}
        @h2xmle_dataFormat  {Q0}
        @h2xmle_default     {1000} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_EX_VI_FTM_GET 0x080013AF

/*==============================================================================
   Type definitions
==============================================================================*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

/** @h2xmlp_subStruct */

typedef struct wsa_ex_vi_ftm_get_param_t wsa_ex_vi_ftm_get_param_t;

struct wsa_ex_vi_ftm_get_param_t {
   int32_t                ftm_Re_q24;
    /**< @h2xmle_description {ftm resitance in ohm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t                ftm_Bl_q24;
    /**< @h2xmle_description {force factor (Bl_product)}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t                ftm_Rms_q24;
    /**< @h2xmle_description {resitance due to mechanical damping, Kg/sec}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t                ftm_Kms_q24;
    /**< @h2xmle_description {suspension stiffness, in N/mm}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q24}
         @h2xmle_default     {0} */
    int32_t                ftm_Fres_q20;
    /**< @h2xmle_description {resonant frequency in Hz}
         @h2xmle_range       {0..2147483647}
         @h2xmle_dataFormat  {Q20}
         @h2xmle_default     {0} */
    uint32_t status; // FTM packet status
    /**< @h2xmle_description {FTM packet status}
         @h2xmle_range       {0...5} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/* Structure definition for Parameter */
typedef struct param_id_haptics_ex_vi_ftm_get param_id_haptics_ex_vi_ftm_get;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_EX_VI_FTM_GET",
                          PARAM_ID_HAPTICS_EX_VI_FTM_GET}
    @h2xmlp_description {Excursion VI FTM mode parameters returned from WSA Smart haptics excursion FTM mode}
    @h2xmlp_toolPolicy  {RTC_READONLY}*/

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_ex_vi_ftm_get {
    uint32_t                 num_channels;
    /**< @h2xmle_description {Number of LRA channels}
         @h2xmle_rangeList   {"1"=1;"2"=2}
         @h2xmle_default     {1} */
#ifdef __H2XML__
    wsa_ex_vi_ftm_get_param_t wsa_ex_vi_ftm_get_param[0];
    /**< @h2xmle_description {structure containing LRA params for FTM }
         @h2xmle_variableArraySize  {num_channels} */
#endif
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_LRA_DIAG_GETPKT_PARAM 0x080013B0

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_lra_diag_getpkt_param_t param_id_haptics_lra_diag_getpkt_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_LRA_DIAG_GETPKT_PARAM",
                          PARAM_ID_HAPTICS_LRA_DIAG_GETPKT_PARAM}
    @h2xmlp_description {Parameter used to get general LRA diagnostics like whether LRA is open, or close or if DC is flowing through it.}
    @h2xmlp_toolPolicy  {RTC_READONLY}*/
// General LRA Diagnostics

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_lra_diag_getpkt_param_t {
    uint32_t num_channels;
    /**< @h2xmle_description {Number of LRA channels}
        @h2xmle_rangeList   {"1"=1;"2"=2}
        @h2xmle_default     {1} */
    uint32_t lra_cond[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {reflects LRA general diagnostics - OK, Short, Open, DC}
         @h2xmle_range       {0...3}
         @h2xmle_dataFormat  {Q0}
         @h2xmle_default     {0} */
    uint32_t temperature_status[HAPTICS_MAX_OUT_CHAN];
    /**< @h2xmle_description {reflects LRA temperature status - OK, above max limit, below min limit}
         @h2xmle_range       {0...2}
         @h2xmle_dataFormat  {Q0}
         @h2xmle_default     {0} */
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_EX_VI_DEMO_PKT_PARAM 0x080013B1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_ex_vi_demo_pkt_param_t param_id_haptics_ex_vi_demo_pkt_param_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_EX_VI_DEMO_PKT_PARAM",
                          PARAM_ID_HAPTICS_EX_VI_DEMO_PKT_PARAM}
    @h2xmlp_description {Parameter used to get general LRA diagnostics like whether LRA is open, or close or if DC is flowing through it.}
    @h2xmlp_toolPolicy  {RTC_READONLY}*/
// General LRA Diagnostics

#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"

struct param_id_haptics_ex_vi_demo_pkt_param_t
{
    uint32_t num_channels;
    /**< @h2xmle_description {Number of LRA channels}
    @h2xmle_rangeList   {"1"=1;"2"=2}
    @h2xmle_default     {1} */
#ifdef __H2XML__
    int32_t xpred_from_vsens[0][MAX_SAMPLES_IN_PACKET];
    /**< @h2xmle_description        {structure containing LRA params for excursion prediction }
         @h2xmle_variableArraySize  {num_channels} */
#endif
}

#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Paramter id */
#define PARAM_ID_HAPTICS_VI_CHANNEL_MAP_CFG 0x080013BE

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct param_id_haptics_vi_channel_map_cfg_t param_id_haptics_vi_channel_map_cfg_t;

/** @h2xmlp_parameter   {"PARAM_ID_HAPTICS_VI_CHANNEL_MAP_CFG",
                         PARAM_ID_HAPTICS_VI_CHANNEL_MAP_CFG}
    @h2xmlp_description {parameter used to set the input channel mapping for Vsens and Isens
                         for the Haptics VI module.}
    @h2xmlp_toolPolicy  {CALIBRATION, RTC_READONLY} */
#include "spf_begin_pack.h"
#include "spf_begin_pragma.h"
struct param_id_haptics_vi_channel_map_cfg_t
{
   uint32_t num_ch;
   /**< @h2xmle_description {Number of channels for Tx signal.}
        @h2xmle_rangeList   {"Two" = 2; "Four" = 4}
        @h2xmle_default     {2} */

   int32_t chan_info[0];
   /**< @h2xmle_description {Channel mapping array that provides information
                             on the order in which the Vsens and Isens of
                             different LRA come into a Tx port.
                             - HAPTICS_VI_VSENS_LEFT_CHAN(1)
                             - HAPTICS_VI_ISENS_LEFT_CHAN(2)
                             - HAPTICS_VI_VSENS_RIGHT_CHAN(3)
                             - HAPTICS_VI_ISENS_RIGHT_CHAN(4)
                      Some possible configurations are (1,2,3,4), (4,2,1,3),
                      (1,2,0,0),(4,3,0,0), and (2,1,0,0).
                      All channels must be unique. The order does not matter
                      as long as the channels are valid.
                      If only two channels are sent (V and I), they must be
                             first two channels: Eg: (4,3), (1,2), etc.}
       @h2xmle_variableArraySize  {num_ch} */
}
#include "spf_end_pragma.h"
#include "spf_end_pack.h"
;

#define EVENT_ID_HAPTICS_VI_CALIBRATION 0x080013B4
/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_parameter   {"EVENT_ID_HAPTICS_VI_CALIBRATION",
                          EVENT_ID_HAPTICS_VI_CALIBRATION}
    @h2xmlp_description { VI calibration event raised by the Haptics Protection Module.}
    @h2xmlp_toolPolicy  { NO_SUPPORT}*/

#define EVENT_ID_HAPTICS_LRA_DIAGNOSTICS 0x080013B3
/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_parameter   {"EVENT_ID_HAPTICS_LRA_DIAGNOSTICS",
                          EVENT_ID_HAPTICS_LRA_DIAGNOSTICS}
    @h2xmlp_description { if change in lra diagnostics state and temperature status,
                          event will be raised by the Haptics Protection Module.}
    @h2xmlp_toolPolicy  { NO_SUPPORT}*/


/** @} <-- End of the Module --> */

#endif /* __WSA_HAPTICS_VI_API_H__ */
