/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file  isa/erbium/fcc.h
 *  \brief Fast Credit Counter (FCC) primitives for Erbium.
 *
 *  Custom `fcc` CSR (0x821) and `fccnb` read CSR match ET-SoC1 — the
 *  wait_fcc / read_fcc / init_fcc helpers are source-identical.
 *
 *  SEND_FCC differs: on Erbium the credit-inc registers live in the
 *  caller's own USER_CPU sub-region (CREDINC0..CREDINC3), not in a
 *  shire-wide ESR region. The `shire` argument is accepted for source
 *  compatibility with ET-SoC1 but ignored.
 */

#ifndef FCC_ERBIUM_H
#define FCC_ERBIUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "isa/erbium/esr_defines.h"
#include "hwinc/esr.h"                 /* USER_CPU_CREDINC0_BYTE_ADDRESS ... */

#define THREAD_0 0
#define THREAD_1 1

typedef enum { FCC_0 = 0, FCC_1 = 1 } fcc_t;

/* Credit-inc layout: 4 registers (CREDINC0..CREDINC3), one per (thread, fcc)
 * pair. Index = thread * 2 + fcc. */
#define SEND_FCC(shire, thread, fcc, bitmask)                                  \
    ((void)(shire),                                                            \
     *(volatile uint64_t *)(ERBIUM_ESR_SELF                                    \
         + USER_CPU_CREDINC0_BYTE_ADDRESS                                      \
         + ((uintptr_t)((thread) * 2 + (fcc))) * sizeof(uint64_t))             \
         = (bitmask))

#define WAIT_FCC(fcc) asm volatile("csrwi fcc, %0" : : "I"(fcc))

static inline void wait_fcc(fcc_t fcc)
{
    asm volatile("csrw fcc, %0" : : "r"(fcc));
}

static inline uint64_t read_fcc(fcc_t fcc)
{
    uint64_t temp;
    uint64_t val;

    asm volatile("   csrr  %0, fccnb  \n"
                 "   beqz  %2, 1f     \n"
                 "   srli  %0, %0, 16 \n"
                 "1: lui   %1, 0x10   \n"
                 "   addiw %1, %1, -1 \n"
                 "   and   %0, %0, %1 \n"
                 : "=&r"(val), "=r"(temp)
                 : "r"(fcc));
    return val;
}

static inline void init_fcc(fcc_t fcc)
{
    for (uint64_t i = read_fcc(fcc); i > 0; i--)
    {
        wait_fcc(fcc);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* FCC_ERBIUM_H */
