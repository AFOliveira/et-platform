/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: IPI trigger/clear ESR semantics
 * Expect: trigger writes OR into ESR (0 writes ignored),
 *         trigger_clear clears bits in ESR, and 0 writes ignored.
 */

#include "test.h"
#include <stdint.h>

#define ESR_IPI_TRIGGER       0x0080F40090ull
#define ESR_IPI_TRIGGER_CLEAR 0x0080F40098ull

static inline uint64_t esr_read(uint64_t addr)
{
    return *(volatile uint64_t *)addr;
}

static inline void esr_write(uint64_t addr, uint64_t value)
{
    *(volatile uint64_t *)addr = value;
}

int main(void)
{
    const uint64_t mask = 0x0006ull; /* avoid hart0 bit */

    /* ESR should start cleared */
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != 0) {
        TEST_FAIL;
    }

    /* trigger sets bits */
    esr_write(ESR_IPI_TRIGGER, mask);
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != mask) {
        TEST_FAIL;
    }

    /* trigger write of 0 is ignored */
    esr_write(ESR_IPI_TRIGGER, 0);
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != mask) {
        TEST_FAIL;
    }

    /* clear one bit */
    esr_write(ESR_IPI_TRIGGER_CLEAR, 0x0002ull);
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != 0x0004ull) {
        TEST_FAIL;
    }

    /* clear write of 0 is ignored */
    esr_write(ESR_IPI_TRIGGER_CLEAR, 0);
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != 0x0004ull) {
        TEST_FAIL;
    }

    /* clear remaining bit */
    esr_write(ESR_IPI_TRIGGER_CLEAR, 0x0004ull);
    if ((esr_read(ESR_IPI_TRIGGER) & 0xFFFFull) != 0x0ull) {
        TEST_FAIL;
    }

    TEST_PASS;
    return 0;
}
