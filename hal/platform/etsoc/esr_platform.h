/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 ESR address encoding.
 *
 *   base           : 0x100000000
 *   bits [31:30]   : PP (privilege)
 *   bits [29:22]   : shire ID
 *   bits [21:0]    : sub-region + register offset
 */

#ifndef PLATFORM_ETSOC1_ESR_PLATFORM_H_
#define PLATFORM_ETSOC1_ESR_PLATFORM_H_

#include <stdint.h>

/* ESR sub-region offsets (bits [21:0] of the address). Values from
 * sw-sysemu/esrs_et.cpp ESR_*_REGION less the 0x100000000 base. */
#define ESR_SR_HART    0x000000U
#define ESR_SR_NEIGH   0x100000U
#define ESR_SR_CACHE   0x300000U
#define ESR_SR_CPU     0x340000U  /* a.k.a. shire_other */

static inline uintptr_t esr_addr(int pp, uint32_t shire,
				 uint32_t subregion, uint32_t offset)
{
	return (uintptr_t)0x100000000ULL
	     | ((uint64_t)(pp & 0x3)     << 30)
	     | ((uint64_t)(shire & 0xFF) << 22)
	     | ((uint64_t)(subregion & 0x3FFFFFU))
	     | ((uint64_t)(offset & 0xFFFFU));
}

#endif /* PLATFORM_ETSOC1_ESR_PLATFORM_H_ */
