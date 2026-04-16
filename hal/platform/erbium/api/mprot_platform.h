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

/*
 * MPROT offset within Machine_neigh.
 * hwinc/esr.h says 0x38 but the emulator and RTL place it at 0x20.
 * Use the hardware-verified value until the RDL is corrected.
 */
#define MPROT_NEIGH_OFFSET 0x20ul

static inline uint64_t mprot_read(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
			    MPROT_NEIGH_OFFSET);
}

static inline void mprot_write(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_NEIGH,
		      MPROT_NEIGH_OFFSET, val);
}

#endif /* ERBIUM_API_MPROT_PLATFORM_H_ */
