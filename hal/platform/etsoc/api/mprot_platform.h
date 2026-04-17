/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 MPROT backend — accesses memory protection via ESR (Machine_neigh).
 */

#ifndef ETSOC_API_MPROT_PLATFORM_H_
#define ETSOC_API_MPROT_PLATFORM_H_

#include <stdint.h>
#include <common/esr.h>
#include <hwinc/shire_esr.h>

#define MPROT_NEIGH_OFFSET M_SHIRE_NEIGH_MPROT_OFFSET

static inline uint64_t platform_mprot_read(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
			    MPROT_NEIGH_OFFSET);
}

static inline void platform_mprot_write(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
		      MPROT_NEIGH_OFFSET, val);
}

#endif /* ETSOC_API_MPROT_PLATFORM_H_ */
