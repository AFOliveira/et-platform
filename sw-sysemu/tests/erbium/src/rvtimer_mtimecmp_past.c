/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: MTIMECMP written "in the past" immediately asserts MTIP.
* Expected: PASS (mip.MTIP set immediately after write)
*/

#include "test.h"
#include <stdint.h>

#define ESR_MTIME              0x80F40200ull
#define ESR_MTIMECMP           0x80F40208ull
#define ESR_MTIME_LOCAL_TARGET 0x80F40218ull

#define MIP_MTIP (1UL << 7)

static inline uint64_t read_mip(void)
{
    uint64_t val;
    asm volatile("csrr %0, mip" : "=r"(val));
    return val;
}

int main(void)
{
    volatile uint64_t *mtime = (volatile uint64_t *)ESR_MTIME;
    volatile uint64_t *mtimecmp = (volatile uint64_t *)ESR_MTIMECMP;
    volatile uint64_t *mtime_target = (volatile uint64_t *)ESR_MTIME_LOCAL_TARGET;

    /* Enable timer interrupt delivery for minion 0. */
    *mtime_target = 0x1;

    /* Start from a known state with no pending timer interrupt. */
    *mtime = 1000;
    *mtimecmp = 1000000;

    if (read_mip() & MIP_MTIP) {
        TEST_FAIL;
    }

    /* Write MTIMECMP "in the past": should assert MTIP immediately. */
    *mtimecmp = 500;

    if (read_mip() & MIP_MTIP) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
