/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file isa/erbium/cacheops.h
 *  \brief Erbium slice of the M-mode cache-ops API.
 *
 *  The neutral bulk comes from isa/common/cacheops.h. Every direct-CSR
 *  cache-op verified identical to ET-SoC1 in Phase 0.
 *
 *  cb_drain is a FENCE on Erbium: there is no shire-cache coalescing
 *  buffer (no matching hwinc or sw-sysemu symbol), so nothing to drain,
 *  but the original ET-SoC1 call sites rely on it for memory ordering —
 *  the fence preserves that intent. Opt-in to a hard compile error via
 *  -DET_PLATFORM_ERBIUM_STRICT_CACHEOPS if a caller should fail loudly.
 */

#ifndef __CACHEOPS_H
#define __CACHEOPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/cacheops.h"

static inline __attribute__((always_inline))
void cb_drain(uint64_t drain_shire, uint64_t drain_bank)
{
    (void)drain_shire;
    (void)drain_bank;
    __asm__ __volatile__("fence" ::: "memory");
}

#ifdef __cplusplus
}
#endif

#endif /* __CACHEOPS_H */
