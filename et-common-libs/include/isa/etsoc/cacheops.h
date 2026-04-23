/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------
*/
/***********************************************************************/
/*! \file isa/etsoc/cacheops.h
    \brief ET-SoC1 slice of the M-mode cache-ops API.

    The platform-neutral bulk of the API (all direct-CSR writes —
    0x7f9/0x7fb/0x7fd/0x7ff, 0x89f/0x8bf/0x8df/0x8ff, 0x81f,
    0x7d0, 0x7e0, 0x810, 0x7d3) lives in isa/common/cacheops.h.
    Only cb_drain needs platform-specific addressing — it pokes a
    shire-wide SC_IDX_COP_SM_CTL_USER ESR that only exists on
    ET-SoC1.
*/
/***********************************************************************/

#ifndef __CACHEOPS_H
#define __CACHEOPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/cacheops.h"
#include "isa/etsoc/esr_defines.h"

/*! \fn  inline void cb_drain(uint64_t drain_shire, uint64_t drain_bank)
    \brief  Drain the coalescing buffer of a shire cache bank.
*/
inline void __attribute__((always_inline)) cb_drain(uint64_t drain_shire, uint64_t drain_bank)
{
    volatile uint64_t *sc_idx_cop_sm_ctl_addr =
        (volatile uint64_t *)ESR_CACHE(drain_shire, drain_bank, SC_IDX_COP_SM_CTL_USER);

    uint64_t state;
    do
    {
        state = (*sc_idx_cop_sm_ctl_addr >> 24) & 0xFF;
    } while (state != 4);

    *sc_idx_cop_sm_ctl_addr = (1 << 0) | (10 << 8);

    do
    {
        state = (*sc_idx_cop_sm_ctl_addr >> 24) & 0xFF;
    } while (state != 4);
}

#ifdef __cplusplus
}
#endif

#endif /* __CACHEOPS_H */
