/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file  isa/erbium/flb.h
 *  \brief Fast Local Barrier (FLB) primitives for Erbium.
 *
 *  The custom `flb` CSR (0x820) matches ET-SoC1 exactly — WAIT_FLB is
 *  source-identical across platforms.
 *
 *  INIT_FLB and READ_FLB differ: on Erbium the barrier counters live in
 *  the caller's own USER_CPU sub-region (per-CPU) rather than a shared
 *  shire-wide ESR region. The `shire` argument is accepted for source
 *  compatibility with ET-SoC1 but ignored.
 */

#ifndef FLB_ERBIUM_H
#define FLB_ERBIUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "isa/erbium/esr_defines.h"
#include "hwinc/esr.h"                 /* USER_CPU_FAST_LOCAL_BARRIER0_BYTE_ADDRESS */

#define INIT_FLB(shire, barrier)                                               \
    ((void)(shire),                                                            \
     *(volatile uint64_t *)(ERBIUM_ESR_SELF                                    \
         + USER_CPU_FAST_LOCAL_BARRIER0_BYTE_ADDRESS                           \
         + (uintptr_t)(barrier) * sizeof(uint64_t)) = 0U)

#define READ_FLB(shire, barrier)                                               \
    ((void)(shire),                                                            \
     *(volatile uint64_t *)(ERBIUM_ESR_SELF                                    \
         + USER_CPU_FAST_LOCAL_BARRIER0_BYTE_ADDRESS                           \
         + (uintptr_t)(barrier) * sizeof(uint64_t)))

#define WAIT_FLB(threads, barrier, result)                                     \
    do                                                                         \
    {                                                                          \
        const uint64_t _val = ((uint64_t)((threads) - 1U) << 5U) + (barrier);  \
        asm volatile("csrrw %0, flb, %1" : "=r"(result) : "r"(_val));          \
    } while (0)

#define FLB_COUNT 32

#ifdef __cplusplus
}
#endif

#endif /* FLB_ERBIUM_H */
