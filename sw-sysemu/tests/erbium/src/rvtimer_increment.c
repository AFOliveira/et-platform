/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: RVTimer MTIME increments over time
* Expected: PASS (MTIME value increases after delay)
*
* HAL-ified variant: uses esr_read_u64/esr_write_u64 from the Erbium
* HAL's platform-abstracted ESR layer. The same source builds for both
* Erbium and ET-SoC1 — only the include path to <platform/esr_platform.h>
* differs (selected via -I).
*/

#include "test.h"
#include <stdint.h>
#include <hwinc/esr.h>
#include <erbium_hal/esr.h>

int main() {
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  MACHINE_CPU_MTIME_ADDRESS, 0);

    uint64_t time1 = esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                                   MACHINE_CPU_MTIME_ADDRESS);

    /* Delay loop - timer should tick during this */
    for (volatile int i = 0; i < 1000; i++) {
        asm volatile("nop");
    }

    uint64_t time2 = esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                                   MACHINE_CPU_MTIME_ADDRESS);

    if (time2 > time1) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
