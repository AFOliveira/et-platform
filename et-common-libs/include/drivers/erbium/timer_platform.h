/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium timer backend — mtime/mtimecmp accessed via the MACHINE_CPU
 * ESR sub-region. mtime_local_target is per-hart and also lives there.
 */

#ifndef _DRIVERS_ERBIUM_TIMER_PLATFORM_H_
#define _DRIVERS_ERBIUM_TIMER_PLATFORM_H_

#include <stdint.h>

#include "isa/common/esr.h"
#include "hwinc/esr.h"          /* MACHINE_CPU_MTIME_ADDRESS etc. */

static inline uint64_t timer_read_mtime(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU, MACHINE_CPU_MTIME_ADDRESS);
}

static inline void timer_write_mtime(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU, MACHINE_CPU_MTIME_ADDRESS, val);
}

static inline uint64_t timer_read_mtimecmp(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU, MACHINE_CPU_MTIME_CMP_ADDRESS);
}

static inline void timer_write_mtimecmp(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU, MACHINE_CPU_MTIME_CMP_ADDRESS, val);
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

#endif /* _DRIVERS_ERBIUM_TIMER_PLATFORM_H_ */
