/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 fast local barrier (FLB) backend.
 */

#ifndef ETSOC_API_BARRIER_PLATFORM_H_
#define ETSOC_API_BARRIER_PLATFORM_H_

#include <stdint.h>
#include <etsoc_hal/inc/minion_csr.h>

#define _MCSR_FLB (MINION_CSR_FLB_ADDRESS)

static inline uint64_t platform_flb_exchange(uint64_t val)
{
	uint64_t rv;

	__asm__ volatile("csrrw %0, %1, %2"
			 : "=r"(rv)
			 : "i"(_MCSR_FLB), "r"(val));
	return rv;
}

#endif /* ETSOC_API_BARRIER_PLATFORM_H_ */
