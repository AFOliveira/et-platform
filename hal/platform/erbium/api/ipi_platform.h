/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium IPI backend — accesses IPI trigger/clear via ESR.
 */

#ifndef ERBIUM_API_IPI_PLATFORM_H_
#define ERBIUM_API_IPI_PLATFORM_H_

#include <stdint.h>
#include <erbium_hal/esr.h>
#include <hwinc/esr.h>

static inline uint64_t ipi_read_trigger(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
			    MACHINE_CPU_IPI_TRIGGER_ADDRESS);
}

static inline void ipi_write_trigger(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      MACHINE_CPU_IPI_TRIGGER_ADDRESS, val);
}

static inline void ipi_write_trigger_clear(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      MACHINE_CPU_IPI_TRIGGER_CLEAR_ADDRESS, val);
}

#endif /* ERBIUM_API_IPI_PLATFORM_H_ */
