/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium ESR address encoding.
 *
 *   bit 31         : always 1 (ESR space marker)
 *   bits [30:24]   : shire ID (Erbium has 1 shire, so effectively always 0)
 *   bits [23:22]   : PP (privilege)
 *   bits [21:0]    : sub-region + register offset
 */

#ifndef PLATFORM_ERBIUM_ESR_PLATFORM_H_
#define PLATFORM_ERBIUM_ESR_PLATFORM_H_

#include <stdint.h>
#include <hwinc/top.h>

static inline uintptr_t esr_addr(int pp, uint32_t shire,
				 uint32_t subregion, uint32_t offset)
{
	return (uintptr_t)ERBIUM_TOP_CPU_REGISTERS_BASE
	     | ((uint64_t)(shire & 0x7F) << 24)
	     | ((uint64_t)(pp & 0x3)     << 22)
	     | ((uint64_t)(subregion & 0x3FFFFFU))
	     | ((uint64_t)(offset & 0xFFFFU));
}

#endif /* PLATFORM_ERBIUM_ESR_PLATFORM_H_ */
