/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Enable all harts and verify they all execute
*
* Each hart writes its ID to MRAM[hartid * 8], then uses FLB (Fast Local
* Barrier) for synchronization. The last hart verifies all locations
* contain expected values.
*/

#include "test.h"
#include "minion_csr_compat.h"
#include <stdint.h>
#include <common/mmio.h>
#include <common/memmap.h>
#include <api/thread.h>

#define MRAM_BASE HAL_MAIN_MEM_BASE
#define NUM_HARTS 16

/*
 * FLB (Fast Local Barrier):
 * - Returns 1 if this hart was the last (triggered wrap), 0 otherwise
 */
static inline int flb_barrier(unsigned barrier_id, unsigned num_harts) {
    uint64_t flb_val =
        MINION_CSR_FLB_BARRIER_NUMBER_SET(barrier_id) |
        MINION_CSR_FLB_MATCH_VALUE_SET(num_harts - 1);
    return minion_csr_swap(FLB, flb_val);
}

int main() {
    volatile uint64_t *mram = (volatile uint64_t *)MRAM_BASE;

    uint64_t hartid = get_hart_id();

    if (hartid == 0) {
        hal_thread_write_thread0_disable(0x00);
        hal_thread_write_thread1_disable(0x00);
    }

    /* Emulator is very predictable, let's introduce
     * some delay so barrier makes sense. */
    if (hartid == 13) {
        for (int i = 0; i < 1000; i++) {
            asm volatile("nop");
        }
    }

    /* Each hart writes its ID to its slot */
    mram[hartid] = hartid + 1;  /* +1 so we can distinguish from unwritten (0) */

    /* Synchronize: wait for all harts to complete their writes */
    int last = flb_barrier(0, NUM_HARTS);

    /* Last hart to arrive verifies all values */
    if (last) {
        for (int i = 0; i < NUM_HARTS; i++) {
            if (mram[i] != (uint64_t)(i + 1)) {
                TEST_FAIL;
            }
        }
    }

    TEST_PASS;
    return 0;
}
