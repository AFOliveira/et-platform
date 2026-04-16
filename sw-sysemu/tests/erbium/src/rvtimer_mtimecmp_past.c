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
#include <erbium_hal/csr.h>
#include <api/timer.h>

#define MIP_MTIP (1UL << 7)

int main(void)
{
    /* Enable timer interrupt delivery for minion 0. */
    timer_write_mtime_local_target(0x1);

    /* Start from a known state with no pending timer interrupt. */
    timer_write_mtime(1000);
    timer_write_mtimecmp(1000000);

    if (csr_read(mip) & MIP_MTIP) {
        TEST_FAIL;
    }

    /* Write MTIMECMP "in the past": should assert MTIP immediately. */
    timer_write_mtimecmp(500);

    if (csr_read(mip) & MIP_MTIP) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
