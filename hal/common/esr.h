/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent ESR access API.
 *
 * Tests and drivers call esr_read_u64() / esr_write_u64() with a logical
 * (pp, shire, subregion, offset) tuple. The actual physical address is
 * computed by the platform-specific esr_addr() function (from
 * <platform/.../esr_platform.h>), which encodes the tuple using the bit
 * layout of the target chip.
 *
 * - Erbium:   base 0x80000000, PP at [23:22], shire at [30:24]
 * - ET-SoC1:  base 0x100000000, PP at [31:30], shire at [29:22]
 *
 * Build system selects the right platform header via -I path.
 */

#ifndef HAL_COMMON_ESR_H_
#define HAL_COMMON_ESR_H_

#include <stdint.h>

/* Privilege domain (PP). Values match the encoding in the address. */
typedef enum {
	ESR_PP_USER       = 0,
	ESR_PP_SUPERVISOR = 1,
	ESR_PP_DEBUG      = 2,
	ESR_PP_MACHINE    = 3,
} esr_pp_t;

/* Sub-region bases (ESR_SR_*) are platform-specific and come from
 * <esr_platform.h>, along with esr_addr(). */
#include <esr_platform.h>

/* ------------------------------------------------------------------ */
/* Typed accessors                                                     */
/* ------------------------------------------------------------------ */

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

#endif /* HAL_COMMON_ESR_H_ */
