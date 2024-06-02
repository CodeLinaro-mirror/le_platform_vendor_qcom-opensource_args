/**
 * \file gpr_pack_begin.h
 * \brief
 *  	This file contains GPR pack begin
 *
 *
 * \copyright
 *  Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 *  SPDX-License-Identifier: BSD-3-Clause-Clear
 */
// clang-format off
/*
$Header$
*/
// clang-format on


#if defined( __qdsp6__ )
/* No packing atrributes for Q6 compiler; all structs manually packed */
#elif defined( __GNUC__ )
#elif defined( __arm__ )
  __packed
#elif defined( _MSC_VER ) && !defined(_KERNEL_MODE)
  #pragma warning( disable:4103 )  /* Another header changing "pack". */
  #pragma pack( push, 1 )
#elif defined( __H2XML__)
#elif defined( _KERNEL_MODE)
#else
  #error "Unsupported compiler."
#endif /* __GNUC__ */
