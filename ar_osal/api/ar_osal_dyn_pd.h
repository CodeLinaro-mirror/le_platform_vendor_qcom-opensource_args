#ifndef AR_OSAL_DYN_PD_H
#define AR_OSAL_DYN_PD_H

/**
 * \file ar_osal_dyn_pd.h
 *
 * \brief
 *      Defines APIs to create dynamic PD
 *      for the given processor domain.
 * \copyright
 *  Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 *  SPDX-License-Identifier: BSD-3-Clause-Clear
 */

/**
 * \brief initialize dynamic pd.
 *
 * \param[in]   proc_id: satellite proc_id.
 *
 * \return
 *  0 -- Success
 *  Nonzero -- Failure
 *
 */
int32_t ar_osal_dyn_pd_init(uint32_t proc_id);

/**
 * \brief de-initialize dynamic pd.
 *
 * \param[in]   proc_id: satellite proc_id.
 *
 * \return
 *  0 -- Success
 *  Nonzero -- Failure
 *
 */
int32_t ar_osal_dyn_pd_deinit(uint32_t proc_id);

#endif
