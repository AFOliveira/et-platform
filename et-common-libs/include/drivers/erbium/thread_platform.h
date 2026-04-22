/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium thread disable backend — via MACHINE_CPU ESR.
 */

#ifndef _DRIVERS_ERBIUM_THREAD_PLATFORM_H_
#define _DRIVERS_ERBIUM_THREAD_PLATFORM_H_

#include <stdint.h>

#include "isa/common/esr.h"
#include "hwinc/esr.h"

static inline void thread_write_thread0_disable(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  MACHINE_CPU_THREAD0_DISABLE_ADDRESS, val);
}

static inline void thread_write_thread1_disable(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  MACHINE_CPU_THREAD1_DISABLE_ADDRESS, val);
}

#endif /* _DRIVERS_ERBIUM_THREAD_PLATFORM_H_ */
