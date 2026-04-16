/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium fast local barrier (FLB) backend.
 */

#ifndef ERBIUM_API_BARRIER_PLATFORM_H_
#define ERBIUM_API_BARRIER_PLATFORM_H_

#include <stdint.h>
#include <hwinc/minion_csr.h>

/* Convert minion_csr byte address to CSR number */
#define _MCSR_FLB ((MINION_CSR_FLB_ADDRESS) / 8)

static inline uint64_t flb_exchange(uint64_t val)
{
	uint64_t rv;

	__asm__ volatile("csrrw %0, %1, %2"
			 : "=r"(rv)
			 : "i"(_MCSR_FLB), "r"(val));
	return rv;
}

#endif /* ERBIUM_API_BARRIER_PLATFORM_H_ */
