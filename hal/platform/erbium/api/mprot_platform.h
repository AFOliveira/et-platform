/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium MPROT backend — accesses memory protection via ESR (Machine_neigh).
 */

#ifndef ERBIUM_API_MPROT_PLATFORM_H_
#define ERBIUM_API_MPROT_PLATFORM_H_

#include <stdint.h>
#include <erbium_hal/esr.h>
#include <hwinc/esr.h>

static inline uint64_t platform_mprot_read(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
			    MACHINE_NEIGH_MPROT_ADDRESS);
}

static inline void platform_mprot_write(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
		      MACHINE_NEIGH_MPROT_ADDRESS, val);
}

#endif /* ERBIUM_API_MPROT_PLATFORM_H_ */
