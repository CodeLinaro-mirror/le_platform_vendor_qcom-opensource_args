#ifndef _AUDIO_IF_API_H_
#define _AUDIO_IF_API_H_
/**
 * \file audio_if_api.h
 * \brief
 *    This file contains audio_if module APIs
 *
 * \copyright
 *  Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *  SPDX-License-Identifier: BSD-3-Clause
 */

#include "hw_intf_cmn_api.h"
//#include "audio_if_dbg_api.h"

/*# @h2xml_title1          {AUDIO IF Module API}
    @h2xml_title_agile_rev {AUDIO IF Module API}
    @h2xml_title_date      {Dec 26, 2024} */

/*==============================================================================
   audio_if related macros
==============================================================================*/

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Input port ID of audio_if module. */
#define PORT_ID_AUDIO_IF_INPUT   0x2

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Output port ID of audio_if module. */
#define PORT_ID_AUDIO_IF_OUTPUT  0x1

/* Stack size for PCM_TDM Module*/
#define AUDIO_IF_STACK_SIZE     2048

/*==============================================================================
   Constants
==============================================================================*/

/* audio_if interface index types */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid indices of Audio Interface.
 */
typedef enum audio_if_idx_t
{
  AUDIO_IF_IDX_INVALID = -1,
  AUDIO_IF_IDX_0       = 0,
  AUDIO_IF_IDX_1,
  AUDIO_IF_IDX_2,
  AUDIO_IF_IDX_3,
  AUDIO_IF_IDX_4,
  AUDIO_IF_IDX_5,
  AUDIO_IF_IDX_6,
  AUDIO_IF_IDX_7,
  AUDIO_IF_IDX_8,
  AUDIO_IF_IDX_9,
  AUDIO_IF_IDX_10,
  AUDIO_IF_IDX_11,
  AUDIO_IF_IDX_12,
  AUDIO_IF_IDX_13,
  AUDIO_IF_IDX_14,
  AUDIO_IF_IDX_15,
  AUDIO_IF_IDX_NUM_MAX
} audio_if_idx_t;

/* audio_if interface modes */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid modes of Audio Interface.
 */
typedef enum audio_if_intf_mode_t
{
  AUDIO_IF_INTF_MODE_TDM = 0,
  AUDIO_IF_INTF_MODE_PCM = 1,
  AUDIO_IF_INTF_MODE_I2S = 2
} audio_if_intf_mode_t;

/* audio_if frame sync mode */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid frame sync modes.
 */
typedef enum audio_if_frame_sync_mode_t
{
  AUDIO_IF_FRAME_SYNC_MODE_SHORT_SYNC    = 0,
  AUDIO_IF_FRAME_SYNC_MODE_ONE_SLOT_SYNC = 1,
  AUDIO_IF_FRAME_SYNC_MODE_LONG_SYNC     = 2
} audio_if_frame_sync_mode_t;

/* audio_if frame sync data delay */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid frame sync data delay.
 */
typedef enum audio_if_frame_sync_data_delay_t
{
  AUDIO_IF_FRAME_SYNC_DATA_DELAY_0BCLK = 0,
  AUDIO_IF_FRAME_SYNC_DATA_DELAY_1BCLK = 1,
  AUDIO_IF_FRAME_SYNC_DATA_DELAY_2BCLK = 2
} audio_if_frame_sync_data_delay_t;

/* audio_if frame sync source */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid frame sync source.
 */
typedef enum audio_if_frame_sync_src_t
{
  AUDIO_IF_FRAME_SYNC_SRC_EXTERNAL = 0,
  /* Frame sync generated from external source */

  AUDIO_IF_FRAME_SYNC_SRC_INTERNAL
  /* Frame sync generated internally by HW */
} audio_if_frame_sync_src_t;

/* audio_if Slots per frame */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid slots per frame.
 */
typedef enum audio_if_slots_per_frame_t
{
  AUDIO_IF_SLOTS_PER_FRAME_1  = 1,
  AUDIO_IF_SLOTS_PER_FRAME_2  = 2,
  AUDIO_IF_SLOTS_PER_FRAME_4  = 4,
  AUDIO_IF_SLOTS_PER_FRAME_8  = 8,
  AUDIO_IF_SLOTS_PER_FRAME_16 = 16,
  AUDIO_IF_SLOTS_PER_FRAME_32 = 32,
  AUDIO_IF_MAX_SLOTS_PER_FRAME = AUDIO_IF_SLOTS_PER_FRAME_32
} audio_if_slots_per_frame_t;

/* audio_if Slot width */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid slot width.
 */
typedef enum audio_if_slot_width_t
{
  AUDIO_IF_SLOT_WIDTH_16 = 16,
  AUDIO_IF_SLOT_WIDTH_24 = 24,
  AUDIO_IF_SLOT_WIDTH_32 = 32
} audio_if_slot_width_t;

/* Bit clock types */

/** @ingroup ar_spf_mod_hwep_common_mods
    Valid bit clock types.
 */
typedef enum audio_if_bit_clk_type_t
{
  AUDIO_IF_I_BIT_CLK_EN = 0,
  /* Enables I bit clock */

  AUDIO_IF_E_BIT_CLK_EN,
  /* Enables E-bit clock */

  AUDIO_IF_SKIP_BIT_CLK_EN
  /* Skips enabling bit clock at AIF.
   *
   * For example, for external clock source selection scenario,
   * client fwk enables the bit clock by interacting
   * with corebsp directly, so need to skip enabling the bit
   * clock from audio driver. */
} audio_if_bit_clk_type_t;

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for disabling the audio_if configuration ctrl_data_out_enable
    parameter. The PCM block is the only master. */
#define AUDIO_IF_CTRL_DATA_OE_DISABLE                           0x0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for enabling the audio_if configuration ctrl_data_out_enable
    parameter. The PCM block shares the signal with other masters. */
#define AUDIO_IF_CTRL_DATA_OE_ENABLE                            0x1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for the Normal synchronization. */
#define AUDIO_IF_FRAME_SYNC_NORMAL                                     0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for inverting the synchronization. */
#define AUDIO_IF_FRAME_SYNC_INVERT                                     1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for the bit clock supply without inversion to the blocks inside QAIF. */
#define AUDIO_IF_INT_CLK_NORMAL                                     0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for inverting the bit clock supply to the blocks inside QAIF. */
#define AUDIO_IF_INT_CLK_INVERT                                     1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for the bit clock supply without inversion to the blocks outside QAIF. */
#define AUDIO_IF_EXT_CLK_NORMAL                                     0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for the bit clock supply with inversion to the blocks outside QAIF. */
#define AUDIO_IF_EXT_CLK_INVERT                                     1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for normal capture clock. */
#define AUDIO_IF_CAPTURE_CLK_NORMAL                                     0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for inverting the capture clock. */
#define AUDIO_IF_CAPTURE_CLK_INVERT                                     1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for the Normal launch clock. */
#define AUDIO_IF_LAUNCH_CLK_NORMAL                                     0

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Enumeration for inverting the launch clock. */
#define AUDIO_IF_LAUNCH_CLK_INVERT                                     1

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Maximum number of channels for the audio_if interface. @newpage */
/*TODO: Need to remove hardcoding after having h2xml fix */
#ifdef PROD_SPECIFIC_MAX_CH
#define AUDIO_IF_MAX_CHANNEL_CNT                                 128
#else
#define AUDIO_IF_MAX_CHANNEL_CNT                                 32
#endif

/*==============================================================================
  Type definition
==============================================================================*/

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Identifier for the parameter that configure the audio_if interface.

    @msgpayload
    param_id_audio_if_intf_cfg_t
 */
#define PARAM_ID_AUDIO_IF_INTF_CFG                              0x08001B11

/*# @h2xmlp_parameter   {"PARAM_ID_AUDIO_IF_INTF_CFG", PARAM_ID_AUDIO_IF_INTF_CFG}
    @h2xmlp_description {ID for the parameter that configures the audio_if
                         interface.}
    @h2xmlp_toolPolicy  {Calibration} */

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Payload for #PARAM_ID_AUDIO_IF_CFG.
 */
#include "spf_begin_pack.h"
struct param_id_audio_if_intf_cfg_t
{
  uint16_t qaif_type;
  /**< QAIF types.

        @valuesbul (See #qaif_type_t)
        - QAIF (Default)
        - @tablebulletend */

  /*#< @h2xmle_description {Audio Interface types.}
        @h2xmle_rangeList   {"QAIF"=0;
                             "QAIF_VA"=1}
        @h2xmle_default     {0} */

  uint16_t intf_idx;
  /**< Index types for the audio_if interface.

        @valuesbul
        - #AUDIO_IF_IDX_0 (Default)
        - #AUDIO_IF_IDX_1
        - #AUDIO_IF_IDX_2
        - #AUDIO_IF_IDX_3
        - #AUDIO_IF_IDX_4
        - #AUDIO_IF_IDX_5
        - #AUDIO_IF_IDX_6
        - #AUDIO_IF_IDX_7
        - #AUDIO_IF_IDX_8
        - #AUDIO_IF_IDX_9
        - #AUDIO_IF_IDX_10
        - #AUDIO_IF_IDX_11
        - #AUDIO_IF_IDX_12
        - #AUDIO_IF_IDX_13
        - #AUDIO_IF_IDX_14
        - #AUDIO_IF_IDX_15 @tablebulletend */

  /*#< @h2xmle_description {Defines audio_if interface type}
        @h2xmle_rangeList   {"AUDIO_IF_IDX_0"=0;
                             "AUDIO_IF_IDX_1"=1;
                             "AUDIO_IF_IDX_2"=2;
                             "AUDIO_IF_IDX_3"=3;
                             "AUDIO_IF_IDX_4"=4;
                             "AUDIO_IF_IDX_5"=5;
                             "AUDIO_IF_IDX_6"=6;
                             "AUDIO_IF_IDX_7"=7;
                             "AUDIO_IF_IDX_8"=8;
                             "AUDIO_IF_IDX_9"=9;
                             "AUDIO_IF_IDX_10"=10;
                             "AUDIO_IF_IDX_11"=11;
                             "AUDIO_IF_IDX_12"=12;
                             "AUDIO_IF_IDX_13"=13;
                             "AUDIO_IF_IDX_14"=14;
                             "AUDIO_IF_IDX_15"=15}
        @h2xmle_default     {0} */


  uint16_t intf_mode;
  /**< Audio interface mode.

        @valuesbul
        - #AUDIO_IF_INTF_MODE_TDM (Default)
        - #AUDIO_IF_INTF_MODE_PCM
        - #AUDIO_IF_INTF_MODE_I2S @tablebulletend */

  /*#< @h2xmle_description {Interface operating mode }
        @h2xmle_rangeList   {"AUDIO_IF_INTF_MODE_TDM"=0;
                             "AUDIO_IF_INTF_MODE_PCM"=1,
                             "AUDIO_IF_INTF_MODE_I2S"=2}
        @h2xmle_default     {0} */

  uint16_t ctrl_data_out_enable;
  /**< Specifies whether the PCM block shares the data-out signal to the
        driver with other masters.

        @valuesbul
        - #AUDIO_IF_CTRL_DATA_OE_DISABLE
        - #AUDIO_IF_CTRL_DATA_OE_ENABLE (Default) @tablebulletend */

  /*#< @h2xmle_description {Specifies whether the PCM block shares the
                             data-out signal to the driver with other masters.}
        @h2xmle_rangeList   {"AUDIO_IF_CTRL_DATA_OE_DISABLE"=0;
                             "AUDIO_IF_CTRL_DATA_OE_ENABLE"=1}
        @h2xmle_default     {1} */

  uint32_t active_slot_mask;
  /**< Specifies the position of the active slot.

        @values 1 through (2^32)-1 (Default = 1)

        Bits 0 to 31 correspond to slots 0 to 31. When a bit is set, the
        corresponding slot is active. The number of active slots can be
        inferred from the number of bits set in the mask.

        32 individual bits can be enabled.*/

  /*#< @h2xmle_description {Specifies position of the active slot}
        @h2xmle_range       {1..4294967295}
        @h2xmle_default     {1} */

  uint16_t nslots_per_frame;
  /**< Number of slots per frame.

        @values 1 through 32 */

  /*#< @h2xmle_description {Number of slots per frame.}
        @h2xmle_rangeList   {"AUDIO_IF_SLOTS_PER_FRAME_1"=1;
                             "AUDIO_IF_SLOTS_PER_FRAME_2"=2;
                             "AUDIO_IF_SLOTS_PER_FRAME_4"=4;
                             "AUDIO_IF_SLOTS_PER_FRAME_8"=8;
                             "AUDIO_IF_SLOTS_PER_FRAME_16"=16;
                             "AUDIO_IF_SLOTS_PER_FRAME_32"=32}
        @h2xmle_default     {1} */

  uint16_t slot_width;
  /**< Width of the slot in a audio_if frame.

        @valuesbul
        - 16 bits (Default)
        - 24 bits (Default)
        - 32 bits @tablebulletend */

  /*#< @h2xmle_description {Width of the slot in a audio_if frame.}
        @h2xmle_rangeList   {"AUDIO_IF_SLOT_WIDTH_16"=16;
                             "AUDIO_IF_SLOT_WIDTH_24"=24;
                             "AUDIO_IF_SLOT_WIDTH_32"=32}
        @h2xmle_default     {16} */

  uint32_t active_lane_mask;
  /**< Position of the active audio_if lanes: 1 to ((2^N)-1), where N is the number
        of physical lanes supported by the hardware interface.

        Bits 0 to N-1 correspond to lanes 0 to N-1. When a bit is set, the
        corresponding lane is active.

        The number of active lanes can be inferred from the number of bits set
        in the mask. */

  /*#< @h2xmle_description {Position of the active audio_if lanes: 1 to ((2^N)-1),
                             where N is the number of physical lanes supported
                             by the hardware interface. \n
                             Bits 0 to N-1 correspond to lanes 0 to N-1. \n
                             When a bit is set, the corresponding lane is
                             active. The number of active lanes can be inferred
                             from the number of bits set in the mask.}
        @h2xmle_range       {1..65535}
        @h2xmle_default     {1} */

  uint32_t frame_sync_rate;
  /**< Defines the sampling rate of frame sync of the interface.

       @values 8, 11.025, 12, 16, 22.05, 24,32, 44.1, 48, 88.2, 96, 176.4,
               192, 352.8, 384 kHz */

  /*#< @h2xmle_description {Defines the sampling rate of frame sync of the interface.
                            For normal scenarios, can choose NATIVE, so, it is assumed
                            that frame sync sample rate same as HW EP media
                            format sample rate.

                            For operating the interface at lower frame sync, then
                            need to set this frame sync sample rate can be different
                            but this frame sync sample rate should be factor or divisor
                            of HW EP media format sample rate. }
       @h2xmle_rangeList   {"NATIVE"=0;
                            "8 kHz"=8000;
                            "11.025 kHz"=11025;
                            "12 kHz"=12000;
                            "16 kHz"=16000;
                            "22.05 kHz"=22050;
                            "24 kHz"=24000;
                            "32 kHz"=32000;
                            "44.1 kHz"=44100;
                            "48 kHz"=48000;
                            "88.2 kHz"=88200;
                            "96 kHz"=96000;
                            "176.4 kHz"=176400;
                            "192 kHz"=192000;
                            "352.8 kHz"=352800;
                            "384 kHz"=384000}
       @h2xmle_default     {0} */

  uint16_t frame_sync_src;
  /**< Specifies the Slave or Master mode of operation.

        @valuesbul
        - #AUDIO_IF_FRAME_SYNC_SRC_EXTERNAL
        - #AUDIO_IF_FRAME_SYNC_SRC_INTERNAL (Default) @tablebulletend */

  /*#< @h2xmle_description {Specifies the Slave or Master mode of operation.}
        @h2xmle_rangeList   {"AUDIO_IF_FRAME_SYNC_SRC_EXTERNAL"=0;
                             "AUDIO_IF_FRAME_SYNC_SRC_INTERNAL"=1}
        @h2xmle_default     {1} */

  uint16_t frame_sync_mode;
  /**< Indicates the audio_if Synchronization mode.

        @valuesbul
        - #AUDIO_IF_FRAME_SYNC_MODE_SHORT_SYNC (Default)
        - #AUDIO_IF_FRAME_SYNC_MODE_ONE_SLOT_SYNC
        - #AUDIO_IF_FRAME_SYNC_MODE_LONG_SYNC @tablebulletend */

  /*#< @h2xmle_description {Indicates the audio_if Synchronization mode.}
        @h2xmle_rangeList   {"AUDIO_IF_FRAME_SYNC_MODE_SHORT_SYNC"=0;
                             "AUDIO_IF_FRAME_SYNC_MODE_ONE_SLOT_SYNC"=1;
                             "AUDIO_IF_FRAME_SYNC_MODE_LONG_SYNC"=2}
        @h2xmle_default     {0} */

  uint16_t invert_frame_sync_pulse;
  /**< Specifies whether to invert synchronization.

        @valuesbul
        - #AUDIO_IF_FRAME_SYNC_NORMAL (Default)
        - #AUDIO_IF_FRAME_SYNC_INVERT @tablebulletend */

  /*#< @h2xmle_description {Specifies whether to invert frame synchronization.}
        @h2xmle_rangeList   {"AUDIO_IF_FRAME_SYNC_NORMAL"=0;
                             "AUDIO_IF_FRAME_SYNC_INVERT"=1}
        @h2xmle_default     {0} */

  uint16_t frame_sync_data_delay;
  /**< Indicates the number of bit-clock cycles when delaying data for
        synchronization.

        @valuesbul
        - #AUDIO_IF_FRAME_SYNC_DATA_DELAY_0BCLK
        - #AUDIO_IF_FRAME_SYNC_DATA_DELAY_1BCLK
        - #AUDIO_IF_FRAME_SYNC_DATA_DELAY_2BCLK @tablebulletend */

  /*#< @h2xmle_description {Indicates the number of bit-clock cycles when
                             delaying data for synchronization.}
        @h2xmle_rangeList   {"AUDIO_IF_FRAME_SYNC_DATA_DELAY_0BCLK"=0;
                             "AUDIO_IF_FRAME_SYNC_DATA_DELAY_1BCLK"=1;
                             "AUDIO_IF_FRAME_SYNC_DATA_DELAY_2BCLK"=2}
        @h2xmle_default     {1} */

  uint16_t bit_clk_type;
  /**< Indicates the bit clock type.

        @valuesbul
        - #AUDIO_IF_I_BIT_CLK_EN (Default)
        - #AUDIO_IF_E_BIT_CLK_EN
        - #AUDIO_IF_SKIP_BIT_CLK_EN @tablebulletend */

  /*#< @h2xmle_description {Indicates the bit clock type.}
        @h2xmle_rangeList   {"AUDIO_IF_I_BIT_CLK_EN"=0;
                             "AUDIO_IF_E_BIT_CLK_EN"=1;
                             "AUDIO_IF_SKIP_BIT_CLK_EN"=2}
        @h2xmle_default     {0} */

  uint8_t inv_int_bit_clk;
  /**< Specifies whether to invert the clock supplied to the blocks inside QAIF.

        @valuesbul
        - #AUDIO_IF_INT_CLK_NORMAL (Default)
        - #AUDIO_IF_INT_CLK_INVERT @tablebulletend */

  /*#< @h2xmle_description {Specifies whether to invert the clock supplied to the blocks inside QAIF.}
        @h2xmle_rangeList   {"AUDIO_IF_INT_CLK_NORMAL"=0;
                             "AUDIO_IF_INT_CLK_INVERT"=1}
        @h2xmle_default     {0} */

  uint8_t inv_ext_bit_clk;
  /**< Specifies whether to invert the clock supplied to the blocks outside of QAIF.

            @valuesbul
        - #AUDIO_IF_EXT_CLK_NORMAL (Default)
        - #AUDIO_IF_EXT_CLK_INVERT @tablebulletend */

  /*#< @h2xmle_description {Specifies whether to invert the clock supplied to the blocks outside of QAIF.}
        @h2xmle_rangeList   {"AUDIO_IF_EXT_CLK_NORMAL"=0;
                             "AUDIO_IF_EXT_CLK_INVERT"=1}
        @h2xmle_default     {0} */
}
#include "spf_end_pack.h"
;
typedef struct param_id_audio_if_intf_cfg_t param_id_audio_if_intf_cfg_t;

typedef enum audio_if_full_cycle_en_t
{
  AUDIO_IF_FULL_CYCLE_DISABLE = 0,
  /* Disable full cycle */

  AUDIO_IF_FULL_CYCLE_ENABLE
  /* Enable full cycle */
} audio_if_full_cycle_en_t;

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Identifier for the parameter that configure the full cycle setting of audio_if interface.

    @msgpayload
    param_id_audio_if_intf_cfg_t
 */
#define PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG                              0x08001B12

/*# @h2xmlp_parameter   {"PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG", PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG}
    @h2xmlp_description {ID for the parameter that configures the audio_if
                         interface full cycle.}
    @h2xmlp_toolPolicy  {Calibration} */

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Payload for #PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG.
 */
#include "spf_begin_pack.h"
struct param_id_audio_if_full_cycle_cfg_t
{
  uint16_t full_cycle_enable;
  /**< Full Cycle configuration enable.

        @valuesbul (See #audio_if_full_cycle_en_t)
        - AUDIO_IF_FULL_CYCLE_DISABLE
        - AUDIO_IF_FULL_CYCLE_ENABLE
        - @tablebulletend */

  /*#< @h2xmle_description {Audio Interface loopback mode.}
        @h2xmle_rangeList   {"AUDIO_IF_FULL_CYCLE_DISABLE"=0;
                              "AUDIO_IF_FULL_CYCLE_ENABLE"=1}
        @h2xmle_default     {0} */

  uint8_t inv_launch_clk;
  /**< Specifies whether to invert the launch clock.

            @valuesbul
        - #AUDIO_IF_LAUNCH_CLK_NORMAL (Default)
        - #AUDIO_IF_LAUNCH_CLK_INVERT @tablebulletend */

  /*#< @h2xmle_description {Specifies whether to invert the launch clock.}
        @h2xmle_rangeList   {"AUDIO_IF_LAUNCH_CLK_NORMAL"=0;
                             "AUDIO_IF_LAUNCH_CLK_INVERT"=1}
        @h2xmle_default     {0} */

  uint8_t inv_capture_clk;
  /**< Specifies whether to invert the capture clock.

            @valuesbul
        - #AUDIO_IF_CAPTURE_CLK_NORMAL (Default)
        - #AUDIO_IF_CAPTURE_CLK_INVERT @tablebulletend */

  /*#< @h2xmle_description {Specifies whether to invert the capture clock.}
        @h2xmle_rangeList   {"AUDIO_IF_CAPTURE_CLK_NORMAL"=0;
                             "AUDIO_IF_CAPTURE_CLK_INVERT"=1}
        @h2xmle_default     {0} */
}
#include "spf_end_pack.h"
;
typedef struct param_id_audio_if_full_cycle_cfg_t param_id_audio_if_full_cycle_cfg_t;

/*------------------------------------------------------------------------------
   Module
------------------------------------------------------------------------------*/

/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Identifier for the hardware audio_if Sink module.

    @subhead4{Supported parameter IDs}
    - #PARAM_ID_AUDIO_IF_INTF_CFG @lstsp1
    - #PARAM_ID_HW_EP_MF_CFG @lstsp1
	- #PARAM_ID_HW_EP_FRAME_DURATION @lstsp1
    - #PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG @lstsp1
    - #PARAM_ID_AUDIO_IF_DBG_LOOPBACK_CFG @lstsp1
    - #PARAM_ID_HW_DELAY


    @subhead4{Supported input media formats}
    - Data formats: @lstsep
       - #DATA_FORMAT_FIXED_POINT (Default) @lstsp2
       - #DATA_FORMAT_IEC61937_PACKETIZED @lstsp2
       - #DATA_FORMAT_IEC60958_PACKETIZED @lstsp2
       - #DATA_FORMAT_DSD_OVER_PCM @lstsp2
       - #DATA_FORMAT_GENERIC_COMPRESSED @lstsp2
       - #DATA_FORMAT_RAW_COMPRESSED @lstsp1
    - fmt_id            : Don't care @lstsp1
    - Sample rates      : 8, 11.025, 12, 16, 22.05, 24, 32, 44.1, 48, 88.2, 96,
                          176.4, 192, 352.8, 384 kHz @lstsp1
    - Number of channels: 1..AUDIO_IF_MAX_CHANNEL_CNT @lstsp1
    - Channel type      : Don't care @lstsp1
    - Bit width: @lstsep
       - 16 bits per sample in Q15 format @lstsp2
       - 24 bits per sample in Q27 format @lstsp2
       - 32 bits per sample in Q31 format @newpage
    - Q format     : Q15 Q27, Q31 @lstsp1
    - Interleaving : De-interleaved unpacked, Interleaved
 */
#define MODULE_ID_AUDIO_IF_SINK                                   0x0700117C

/*# @h2xmlm_module                {"MODULE_ID_AUDIO_IF_SINK", MODULE_ID_AUDIO_IF_SINK}
    @h2xmlm_displayName           {"Audio_if Sink"}
    @h2xmlm_modSearchKeys         {hardware}
    @h2xmlm_description           {ID for the Audio If Sink module. For more
                                   details, see AudioReach Signal Processing Framework
                                   (SPF) API Reference.}
    @h2xmlm_dataInputPorts        {IN=PORT_ID_AUDIO_IF_INPUT}
    @h2xmlm_dataMaxInputPorts     {1}
    @h2xmlm_dataMaxOutputPorts    {0}
    @h2xmlm_ctrlDynamicPortIntent {"Qtimer HWEP drift info"=INTENT_ID_TIMER_DRIFT_INFO,
                                   maxPorts= -1}
    @h2xmlm_ctrlDynamicPortIntent {"HW EP Module Instance info"=INTENT_ID_MODULE_INSTANCE_INFO,
                                   maxPorts= -1}
    @h2xmlm_supportedContTypes    {APM_CONTAINER_TYPE_GC,APM_CONTAINER_TYPE_PTC}
    @h2xmlm_stackSize             {AUDIO_IF_STACK_SIZE}

    @{                      <-- Start of the Module -->
    @h2xml_Select           {param_id_hw_ep_mf_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_mf_t::num_channels}
    @h2xmle_range           {1..AUDIO_IF_MAX_CHANNEL_CNT}
    @h2xml_Select           {param_id_hw_ep_mf_t::data_format}
    @h2xmle_rangeList       {"DATA_FORMAT_FIXED_POINT"=1}
    @h2xmle_default         {1}
    @h2xml_Select           {param_id_audio_if_intf_cfg_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_frame_duration_t}
    @h2xmle_range           {1..100000}
	  @h2xmle_default         {1}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_delay_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_audio_if_full_cycle_cfg_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_audio_if_dbg_loopback_cfg_t}
    @h2xmlm_InsertParameter
    @}                      <-- End of the Module --> */


/** @ingroup ar_spf_mod_hwep_audio_if_mods
    Identifier for the audio_if Source module. The module does not take any inputs.

    @subhead4{Supported parameter IDs}
    - #PARAM_ID_AUDIO_IF_INTF_CFG @lstsp1
    - #PARAM_ID_HW_EP_MF_CFG @lstsp1
	- #PARAM_ID_HW_EP_FRAME_DURATION @lstsp1
    - #PARAM_ID_HW_EP_SRC_OUT_DATA_INTERLEAVING @lstsp1
    - #PARAM_ID_HW_EP_TIMESTAMP_PROPAGATION  @lstsp1
    - #PARAM_ID_AUDIO_IF_FULL_CYCLE_CFG @lstsp1
    - #PARAM_ID_AUDIO_IF_DBG_LOOPBACK_CFG @lstsp1
    - PARAM_ID_HW_DELAY

 */
#define MODULE_ID_AUDIO_IF_SOURCE                           0x0700117D

/*# @h2xmlm_module                {"MODULE_ID_AUDIO_IF_SOURCE",MODULE_ID_AUDIO_IF_SOURCE}
    @h2xmlm_displayName           {"Audio_if Source"}
    @h2xmlm_modSearchKeys         {hardware}
    @h2xmlm_description           {ID for the audio_if Source module. For more
                                   details, see AudioReach Signal Processing Framework
                                   (SPF) API Reference.}
    @h2xmlm_dataOutputPorts       {OUT=PORT_ID_AUDIO_IF_OUTPUT}
    @h2xmlm_dataMaxInputPorts     {0}
    @h2xmlm_dataMaxOutputPorts    {1}
    @h2xmlm_ctrlDynamicPortIntent {"Qtimer HWEP drift info"=INTENT_ID_TIMER_DRIFT_INFO,
                                   maxPorts= -1}
    @h2xmlm_ctrlDynamicPortIntent {"HW EP Module Instance info"=INTENT_ID_MODULE_INSTANCE_INFO,
                                   maxPorts= -1}
    @h2xmlm_supportedContTypes    {APM_CONTAINER_TYPE_GC,APM_CONTAINER_TYPE_PTC}
    @h2xmlm_stackSize             {AUDIO_IF_STACK_SIZE}

    @{                      <-- Start of the Module -->
    @h2xml_Select           {param_id_audio_if_intf_cfg_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_mf_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_mf_t::num_channels}
    @h2xmle_range           {1..AUDIO_IF_MAX_CHANNEL_CNT}
    @h2xml_Select           {param_id_hw_ep_mf_t::data_format}
    @h2xmle_rangeList       {"DATA_FORMAT_FIXED_POINT"=1}
    @h2xmle_default         {1}
    @h2xml_Select           {param_id_hw_ep_frame_duration_t}
    @h2xmle_range           {1..100000}
	  @h2xmle_default         {1}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_delay_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_data_interleaving_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_hw_ep_timestamp_propagation_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_audio_if_full_cycle_cfg_t}
    @h2xmlm_InsertParameter
    @h2xml_Select           {param_id_audio_if_dbg_loopback_cfg_t}
    @h2xmlm_InsertParameter
    @}                      <-- End of the Module --> */


#endif // _AUDIO_IF_API_H_
