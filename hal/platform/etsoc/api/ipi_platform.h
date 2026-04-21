/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 IPI backend — accesses IPI trigger/clear via ESR shire_other.
 */

#ifndef ETSOC_API_IPI_PLATFORM_H_
#define ETSOC_API_IPI_PLATFORM_H_

#include <stdint.h>
#include <common/esr.h>
#include <hwinc/etsoc_shire_other_esr.h>

#define HAL_IPI_TRIGGER_MASK \
	ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_IPI_TRIGGER_FIELD_MASK

static inline uint64_t platform_ipi_read_trigger(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
			    ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_ADDRESS);
}

static inline void platform_ipi_write_trigger(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_ADDRESS, val);
}

static inline void platform_ipi_write_trigger_clear(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_CLEAR_ADDRESS, val);
}

#endif /* ETSOC_API_IPI_PLATFORM_H_ */
