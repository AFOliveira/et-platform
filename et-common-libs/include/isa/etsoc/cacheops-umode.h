/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------
*/
/***********************************************************************/
/*! \file isa/etsoc/cacheops-umode.h
    \brief ET-SoC1 slice of the U-mode cache-ops API.

    The platform-neutral bulk of the API (syscall wrappers, VA-based
    csrw inlines, UCACHE control) lives in isa/common/cacheops-umode.h.
    Only cache_ops_cb_drain needs platform-specific addressing — it
    reaches into a shire-wide cache-bank ESR region that exists only on
    ET-SoC1.
*/
/***********************************************************************/

#ifndef __CACHEOPS_UMODE_H
#define __CACHEOPS_UMODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/cacheops-umode.h"
#include "isa/etsoc/esr_defines.h"

/*! \fn  inline void cache_ops_cb_drain(uint64_t drain_shire, uint64_t drain_bank)
    \brief  Drain the coalescing buffer of a shire cache bank.
    \param drain_shire  shire to drain
    \param drain_bank   shire cache bank to drain
*/
inline void __attribute__((always_inline))
cache_ops_cb_drain(uint64_t drain_shire, uint64_t drain_bank)
{
    // 1. Write the CB invalidate (assumes FSM always available)
    volatile uint64_t *sc_idx_cop_sm_ctl_addr =
        (volatile uint64_t *)ESR_CACHE(drain_shire, drain_bank, SC_IDX_COP_SM_CTL_USER);

    // 2. Checks done
    uint64_t state;
    do
    {
        state = (*sc_idx_cop_sm_ctl_addr >> 24) & 0xFF;
    } while (state != 4);

    *sc_idx_cop_sm_ctl_addr = (1 << 0) | // Go bit = 1
                              (10 << 8); // Opcode = CB_Inv (Coalescing buffer invalidate)

    // 3. Checks done
    do
    {
        state = (*sc_idx_cop_sm_ctl_addr >> 24) & 0xFF;
    } while (state != 4);
}

#ifdef __cplusplus
}
#endif

#endif // ! __CACHEOPS_UMODE_H
