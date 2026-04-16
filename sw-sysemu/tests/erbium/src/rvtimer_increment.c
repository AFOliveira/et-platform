/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: RVTimer MTIME increments over time
* Expected: PASS (MTIME value increases after delay)
*/

#include "test.h"
#include <stdint.h>
#include <api/timer.h>

int main() {
    timer_write_mtime(0);

    uint64_t time1 = timer_read_mtime();

    /* Delay loop - timer should tick during this */
    for (volatile int i = 0; i < 1000; i++) {
        asm volatile("nop");
    }

    uint64_t time2 = timer_read_mtime();

    if (time2 > time1) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
