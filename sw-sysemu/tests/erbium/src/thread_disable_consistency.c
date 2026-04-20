/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Verify threadX_disable ESR writes correctly disable harts
*
* Expected (bug fixed): Only H0 continues, signals PASS
* Actual (bug present): H1 or others continue, signal FAIL
*/

#include "test.h"
#include <stdint.h>
#include <common/mmio.h>
#include <common/memmap.h>
#include <api/thread.h>

#define MRAM_BASE HAL_MAIN_MEM_BASE
#define MARKERS   (MRAM_BASE + 0x100)

int main() {
    volatile uint64_t *markers = (volatile uint64_t *)MARKERS;

    uint64_t hartid = get_hart_id();

    /* Phase 1: H0 initializes and enables all even harts */
    if (hartid == 0) {
        for (int i = 0; i < 16; i++) {
            markers[i] = 0;
        }
        hal_thread_write_thread0_disable(0x00);
        hal_thread_write_thread1_disable(0xFF);
    }

    /* Let all harts sync up */
    for (volatile int i = 0; i < 100; i++) {
        asm volatile("nop");
    }

    /*
     * Phase 2: All harts write 0xFE to thread0_disable
     * 0xFE: bit 0 = 0 (H0 enabled), bits 1-7 = 1 (H2-H14 disabled)
     */
    hal_thread_write_thread0_disable(0xFE);

    /* Give a moment for harts to get disabled (immediate on emulator) */
    for (volatile int i = 0; i < 200; i++) {
        asm volatile("nop");
    }

    /* Phase 3: Mark presence - whoever runs writes their marker */
    markers[hartid] = hartid + 1;

    /*
     * Phase 4: Each hart checks if it should be running
     *
     * After writing 0xFE, only H0 should continue.
     * Any other hart reaching here means the bug is present.
     */
    if (hartid != 0) {
        /* BUG: This hart (H2, H4, etc.) should have been disabled! */
        TEST_FAIL;
        return 1;
    }

    /* Double-check: only our marker should be set */
    for (int i = 2; i < 16; i += 2) {
        if (markers[i] != 0) {
            TEST_FAIL;  /* A disabled hart wrote! */
        }
    }

    TEST_PASS;
    return 0;
}
