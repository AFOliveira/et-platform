/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file isa/erbium/cacheops-umode.h
 *  \brief Erbium slice of the U-mode cache-ops API.
 *
 *  The neutral bulk comes from isa/common/cacheops-umode.h. Every
 *  custom cache-ops CSR used there was verified identical between
 *  ET-SoC1 and Erbium in the Phase 0 recon.
 *
 *  cache_ops_cb_drain is intentionally NOT ported yet: ET-SoC1's
 *  implementation pokes a shire-wide SC_IDX_COP_SM_CTL_USER ESR inside
 *  the shire-cache region. Erbium's memory subsystem differs and the
 *  equivalent addressing has not been investigated. Callers that need
 *  it on Erbium should fail loudly rather than silently do nothing.
 */

#ifndef __CACHEOPS_UMODE_H
#define __CACHEOPS_UMODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/cacheops-umode.h"

static inline __attribute__((always_inline))
void cache_ops_cb_drain(uint64_t drain_shire, uint64_t drain_bank)
{
    (void)drain_shire;
    (void)drain_bank;
#if defined(ET_PLATFORM_ERBIUM_STRICT_CACHEOPS)
#  error "cache_ops_cb_drain: no Erbium implementation yet"
#endif
}

#ifdef __cplusplus
}
#endif

#endif // ! __CACHEOPS_UMODE_H
