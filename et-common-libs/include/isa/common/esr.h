/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent ESR access API.
 *
 * Callers use esr_read_u64() / esr_write_u64() with a logical
 * (pp, shire, subregion, offset) tuple. The physical address is
 * computed by the platform-specific esr_addr() function supplied by
 * isa/<platform>/esr_platform.h, selected here via ET_PLATFORM_ERBIUM.
 *
 *   Erbium:   base 0x80000000, PP [23:22], shire [30:24]
 *   ET-SoC1:  base 0x100000000, PP [31:30], shire [29:22]
 */

#ifndef _ISA_COMMON_ESR_H_
#define _ISA_COMMON_ESR_H_

#include <stdint.h>

/* Privilege domain (PP). Values match the encoding in the address. */
typedef enum {
    ESR_PP_USER       = 0,
    ESR_PP_SUPERVISOR = 1,
    ESR_PP_DEBUG      = 2,
    ESR_PP_MACHINE    = 3,
} esr_pp_t;

/* Sub-region bases inside bits [21:0] of the ESR address.
 * Same layout on Erbium and ET-SoC1 — only the PP/shire bit positions
 * differ, which is handled by esr_addr() in the per-platform header. */
#define ESR_SR_HART    0x000000U  /* Hart debug sub-region */
#define ESR_SR_NEIGH   0x100000U  /* Neighbourhood sub-region */
#define ESR_SR_CPU     0x340000U  /* CPU (shire-wide) sub-region */

#ifdef ET_PLATFORM_ERBIUM
#include "isa/erbium/esr_platform.h"
#else
#include "isa/etsoc/esr_platform.h"
#endif

static inline uint64_t esr_read_u64(esr_pp_t pp, uint32_t shire,
                                    uint32_t subregion, uint32_t offset)
{
    return *(volatile uint64_t *)esr_addr(pp, shire, subregion, offset);
}

static inline void esr_write_u64(esr_pp_t pp, uint32_t shire,
                                 uint32_t subregion, uint32_t offset,
                                 uint64_t val)
{
    *(volatile uint64_t *)esr_addr(pp, shire, subregion, offset) = val;
}

#endif /* _ISA_COMMON_ESR_H_ */
