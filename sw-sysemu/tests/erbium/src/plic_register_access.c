/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Basic PLIC register access
* Expected: PASS
*
* This test verifies that PLIC registers are accessible at the expected
* address and basic read/write operations work correctly.
*/

#include "test.h"
#include <stdint.h>
#include <erbium_hal/csr.h>
#include <api/plic.h>

/* Test constants */
#define TEST_SOURCE_ID          1
#define TEST_CONTEXT_ID         0
#define INVALID_CONTEXT_ID      31

/* Validation1 diagnostics */
#define ET_DIAG_IRQ_INJ         0x5ULL
#define ET_DIAG_IRQ_INJ_PLIC    0x3ULL

static inline void plic_diag_set_pending(uint32_t source_id, uint32_t raise) {
    uint64_t cmd = (ET_DIAG_IRQ_INJ << 56)
                 | ((uint64_t)(raise & 1) << 55)
                 | (ET_DIAG_IRQ_INJ_PLIC << 53)
                 | source_id;
    csr_write(validation1, cmd);
}

int main() {
    uint32_t val;

    /*
     * Test 1: Priority register write/read
     */
    plic_write_priority(TEST_SOURCE_ID, 5);
    val = plic_read_priority(TEST_SOURCE_ID);
    if (val != 5) {
        TEST_FAIL;
    }

    plic_write_priority(TEST_SOURCE_ID, 0);
    val = plic_read_priority(TEST_SOURCE_ID);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 1b: Source 0 priority is hardwired to 0.
     */
    plic_write_priority(0, 7);
    val = plic_read_priority(0);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 2: Enable register write/read
     */
    plic_write_enable(TEST_CONTEXT_ID, 0, (1U << TEST_SOURCE_ID));
    val = plic_read_enable(TEST_CONTEXT_ID, 0);
    if (val != (1U << TEST_SOURCE_ID)) {
        TEST_FAIL;
    }

    plic_write_enable(TEST_CONTEXT_ID, 0, 0);
    val = plic_read_enable(TEST_CONTEXT_ID, 0);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 2a: Out-of-range enable word access must be ignored.
     */
    plic_write_enable(TEST_CONTEXT_ID, 1, 0xFFFFFFFFu);
    val = plic_read_enable(TEST_CONTEXT_ID, 1);
    if (val != 0) {
        TEST_FAIL;
    }
    val = plic_read_enable(TEST_CONTEXT_ID, 0);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 2b: Writes to unmapped context must not alias context 0.
     */
    plic_write_enable(INVALID_CONTEXT_ID, 0, (1U << TEST_SOURCE_ID));
    val = plic_read_enable(TEST_CONTEXT_ID, 0);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 3: Threshold register write/read
     */
    plic_write_threshold(TEST_CONTEXT_ID, 3);
    val = plic_read_threshold(TEST_CONTEXT_ID);
    if (val != 3) {
        TEST_FAIL;
    }

    plic_write_threshold(TEST_CONTEXT_ID, 0);
    val = plic_read_threshold(TEST_CONTEXT_ID);
    if (val != 0) {
        TEST_FAIL;
    }

    /*
     * Test 4: Claim clears pending for claimed source.
     */
    plic_write_priority(TEST_SOURCE_ID, 1);
    plic_write_enable(TEST_CONTEXT_ID, 0, (1U << TEST_SOURCE_ID));
    plic_diag_set_pending(TEST_SOURCE_ID, 1);

    val = plic_claim(TEST_CONTEXT_ID);
    if (val != TEST_SOURCE_ID) {
        TEST_FAIL;
    }
    val = plic_claim(TEST_CONTEXT_ID);
    if (val != 0) {
        TEST_FAIL;
    }
    plic_complete(TEST_CONTEXT_ID, TEST_SOURCE_ID);
    val = plic_claim(TEST_CONTEXT_ID);
    if (val != 0) {
        TEST_FAIL;
    }
    plic_diag_set_pending(TEST_SOURCE_ID, 0);

    /*
     * Test 4b: Out-of-range completion ID write must be ignored.
     */
    plic_diag_set_pending(TEST_SOURCE_ID, 1);
    val = plic_claim(TEST_CONTEXT_ID);
    if (val != TEST_SOURCE_ID) {
        TEST_FAIL;
    }
    plic_complete(TEST_CONTEXT_ID, 0xFFFFFFFFu);
    val = plic_claim(TEST_CONTEXT_ID);
    if (val != 0) {
        TEST_FAIL;
    }
    plic_complete(TEST_CONTEXT_ID, TEST_SOURCE_ID);
    val = plic_claim(TEST_CONTEXT_ID);
    if (val != 0) {
        TEST_FAIL;
    }
    plic_diag_set_pending(TEST_SOURCE_ID, 0);

    plic_write_enable(TEST_CONTEXT_ID, 0, 0);
    plic_write_priority(TEST_SOURCE_ID, 0);

    /*
     * Test 5: Pending register is read-only
     */
    val = plic_read_pending(0);
    if (val != 0) {
        TEST_FAIL;
    }

    val = plic_read_pending(1);
    if (val != 0) {
        TEST_FAIL;
    }

    TEST_PASS;
    return 0;
}
