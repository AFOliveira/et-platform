/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium timer backend — accesses mtime/mtimecmp via ESR.
 */

#ifndef ERBIUM_API_TIMER_PLATFORM_H_
#define ERBIUM_API_TIMER_PLATFORM_H_

#include <stdint.h>
#include <erbium_hal/esr.h>
#include <hwinc/esr.h>

static inline uint64_t timer_read_mtime(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
			    MACHINE_CPU_MTIME_ADDRESS);
}

static inline void timer_write_mtime(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      MACHINE_CPU_MTIME_ADDRESS, val);
}

static inline uint64_t timer_read_mtimecmp(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
			    MACHINE_CPU_MTIME_CMP_ADDRESS);
}

static inline void timer_write_mtimecmp(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      MACHINE_CPU_MTIME_CMP_ADDRESS, val);
}

static inline uint64_t timer_read_mtime_local_target(void)
{
	return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
			    MACHINE_CPU_MTIME_LOCAL_TARGET_ADDRESS);
}

static inline void timer_write_mtime_local_target(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      MACHINE_CPU_MTIME_LOCAL_TARGET_ADDRESS, val);
}

#endif /* ERBIUM_API_TIMER_PLATFORM_H_ */
