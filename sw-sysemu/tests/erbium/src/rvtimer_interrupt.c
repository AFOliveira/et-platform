/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: RVTimer interrupt fires when MTIME >= MTIMECMP
* Expected: PASS (machine timer interrupt fires, trap handler catches it)
*/

#include "test.h"
#include "trap.h"
#include <stdint.h>
#include <erbium_hal/csr.h>
#include <api/timer.h>

#define MIE_MTIE    (1UL << 7)
#define MSTATUS_MIE (1UL << 3)

int main() {
    /* Enable timer interrupt for minion 0 only */
    timer_write_mtime_local_target(0x1);

    timer_write_mtime(0);

    /* Set MTIMECMP to fire after 100 timer ticks */
    timer_write_mtimecmp(100);

    expect_exception(CAUSE_MACHINE_TIMER_INTERRUPT);

    /* Enable machine timer interrupt in MIE */
    csr_set(mie, MIE_MTIE);

    /* Enable global machine interrupts in MSTATUS */
    csr_set(mstatus, MSTATUS_MIE);

    /* Wait for interrupt - should fire within a few thousand cycles */
    for (volatile int i = 0; i < 100000; i++) {
        asm volatile("nop");
    }

    TEST_FAIL;
    return 0;
}
