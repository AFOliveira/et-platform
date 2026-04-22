/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file include/isa/erbium/utils.h
 *  \brief Erbium slice of the shared ISA utils header.
 *
 *  Mirrors isa/etsoc/utils.h: platform-neutral bulk comes from
 *  isa/common/utils.h; the two ESR-backed helpers (fcc_send,
 *  flbarrier_set) use Erbium's per-CPU USER_CPU sub-region rather
 *  than a shared shire-wide ESR region. The `shire` argument is
 *  accepted for source compatibility with ET-SoC1 but ignored.
 */
#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/utils.h"
#include "isa/erbium/esr_defines.h"
#include "hwinc/esr.h"   /* USER_CPU_CREDINC0_BYTE_ADDRESS, USER_CPU_FAST_LOCAL_BARRIER0_BYTE_ADDRESS */

static inline __attribute__((always_inline)) void fcc_send(
    uint32_t shire, uint32_t thread, uint32_t fcc_reg, uint64_t hart_mask)
{
    (void)shire;
    *(volatile uint64_t *)(ERBIUM_ESR_SELF
        + USER_CPU_CREDINC0_BYTE_ADDRESS
        + ((uintptr_t)((thread << 1) | fcc_reg)) * sizeof(uint64_t)) = hart_mask;
}

static inline __attribute__((always_inline)) void flbarrier_set(
    uint32_t shire, uint32_t barrier_num, uint64_t value)
{
    (void)shire;
    *(volatile uint64_t *)(ERBIUM_ESR_SELF
        + USER_CPU_FAST_LOCAL_BARRIER0_BYTE_ADDRESS
        + (uintptr_t)barrier_num * sizeof(uint64_t)) = value;
}

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_H_ */
