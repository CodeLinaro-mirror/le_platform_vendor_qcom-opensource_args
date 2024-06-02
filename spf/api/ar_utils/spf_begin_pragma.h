/**
 * \file spf_begin_pragma.h
 * \brief 
 *  	 This file defines pragma to ignore zero length array warnings for different compilers
 * 
 * \copyright
 *  Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 *  SPDX-License-Identifier: BSD-3-Clause-Clear
 */
// clang-format off
/*
$Header: //components/rel/avs.fwk/1.0/api/ar_utils/spf_begin_pragma.h#3 $
*/
// clang-format on
 

#if defined( __qdsp6__ )
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wzero-length-array"
#elif defined( __GNUC__ )
#elif defined( __arm__ )
#elif defined( _MSC_VER )
  #pragma warning( push )
  #pragma warning( disable:4200 )
#elif defined( __H2XML__)
#else
  #error "Unsupported compiler."
#endif
