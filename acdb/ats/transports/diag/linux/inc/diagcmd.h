#ifndef DIAGCMD_H
#define DIAGCMD_H
/**
*==============================================================================
* \file diagcmd.h
* \brief
*                   D I A G N O S T I C  C O M M A N D
*                       I N T E R F A C E  S T U B
*
*           This file is a stub version of the Diagnostic System Command
*           Interface header. This file is used for compilation with the
*           Off-target Test Platform(OTTP).
*
* \copyright
*  Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
*  SPDX-License-Identifier: BSD-3-Clause-Clear
*
*==============================================================================
*/

/* Invalid Command Response                   */
#define DIAG_BAD_CMD_F  19

typedef enum {
  DIAG_SUBSYS_AUDIO_SETTINGS     = 17,      /* Audio Settings */
} diagpkt_subsys_cmd_enum_type;

#endif  /* DIAGCMD_H */



