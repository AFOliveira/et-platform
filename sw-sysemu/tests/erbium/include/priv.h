/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#ifndef ERBIUM_PRIV_H
#define ERBIUM_PRIV_H

#include <stdint.h>
#include <common/memmap.h>
#include <api/mprot.h>

/* MPROT field layout is Erbium-only; ET-SoC1's MPROT lives in a
 * different hwinc header with unrelated field names. */
#if defined(PLATFORM_IS_ERBIUM)
#  include <hwinc/esr.h>
#  define MPROT_EN            MACHINE_NEIGH_MPROT_MPROT_EN_FIELD_MASK
#  define MPROT_MMODE_SIZE(n) MACHINE_NEIGH_MPROT_MMODE_SIZE_SET(n)
#  define MPROT_SMODE_SIZE(n) MACHINE_NEIGH_MPROT_SMODE_SIZE_SET(n)
#else
#  define MPROT_EN            (1 << 8)
#  define MPROT_MMODE_SIZE(n) ((n) << 4)
#  define MPROT_SMODE_SIZE(n) ((n) << 0)
#endif

#define MRAM_BASE  HAL_MAIN_MEM_BASE

/*
 * Run callback in S-mode.
 * The callback must not return - it should call TEST_PASS/TEST_FAIL
 * or trigger a trap that ends the test.
 */
__attribute__((noinline, noreturn, unused))
static void run_in_smode(void (*func)(void)) {
    asm volatile(
        "csrw mepc, %0\n"
        "li t0, (3 << 11)\n"    /* MPP mask */
        "csrc mstatus, t0\n"    /* clear MPP */
        "li t0, (1 << 11)\n"    /* MPP = S-mode */
        "csrs mstatus, t0\n"
        "mret\n"
        :
        : "r"(func)
        : "t0"
    );
    __builtin_unreachable();
}

/*
 * Run callback in U-mode.
 * The callback must not return - it should call TEST_PASS/TEST_FAIL
 * or trigger a trap that ends the test.
 */
__attribute__((noinline, noreturn, unused))
static void run_in_umode(void (*func)(void)) {
    asm volatile(
        "csrw mepc, %0\n"
        "li t0, (3 << 11)\n"    /* MPP mask */
        "csrc mstatus, t0\n"    /* clear MPP (00 = U-mode) */
        "mret\n"
        :
        : "r"(func)
        : "t0"
    );
    __builtin_unreachable();
}

#endif /* ERBIUM_PRIV_H */
