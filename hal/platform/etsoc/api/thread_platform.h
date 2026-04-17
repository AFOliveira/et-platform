/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 thread disable backend — via ESR shire_other.
 */

#ifndef ETSOC_API_THREAD_PLATFORM_H_
#define ETSOC_API_THREAD_PLATFORM_H_

#include <stdint.h>
#include <common/esr.h>
#include <hwinc/etsoc_shire_other_esr.h>

static inline void platform_thread_write_thread0_disable(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      ETSOC_SHIRE_OTHER_ESR_THREAD0_DISABLE_ADDRESS, val);
}

static inline void platform_thread_write_thread1_disable(uint64_t val)
{
	esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
		      ETSOC_SHIRE_OTHER_ESR_THREAD1_DISABLE_ADDRESS, val);
}

#endif /* ETSOC_API_THREAD_PLATFORM_H_ */
