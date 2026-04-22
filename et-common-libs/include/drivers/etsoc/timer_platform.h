/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 timer backend. mtime/mtimecmp live in the per-hart RVTIMER
 * block in the HART ESR sub-region (hwinc/pu_rvtim.h). mtime_local_target
 * lives in the shire_other ESR sub-region
 * (hwinc/etsoc_shire_other_esr.h).
 */

#ifndef _DRIVERS_ETSOC_TIMER_PLATFORM_H_
#define _DRIVERS_ETSOC_TIMER_PLATFORM_H_

#include <stdint.h>

#include "isa/common/esr.h"
#include "hwinc/pu_rvtim.h"
#include "hwinc/etsoc_shire_other_esr.h"

static inline uint64_t timer_read_mtime(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_HART, RVTIMER_MTIME_ADDRESS);
}

static inline void timer_write_mtime(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_HART, RVTIMER_MTIME_ADDRESS, val);
}

static inline uint64_t timer_read_mtimecmp(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_HART, RVTIMER_MTIMECMP_ADDRESS);
}

static inline void timer_write_mtimecmp(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_HART, RVTIMER_MTIMECMP_ADDRESS, val);
}

static inline uint64_t timer_read_mtime_local_target(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                        ETSOC_SHIRE_OTHER_ESR_MTIME_LOCAL_TARGET_ADDRESS);
}

static inline void timer_write_mtime_local_target(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  ETSOC_SHIRE_OTHER_ESR_MTIME_LOCAL_TARGET_ADDRESS, val);
}

#endif /* _DRIVERS_ETSOC_TIMER_PLATFORM_H_ */
