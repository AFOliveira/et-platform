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
#include <api/ipi.h>

int main(void)
{
    const uint64_t mask = 0x0006ull; /* avoid hart0 bit */

    /* ESR should start cleared */
    if ((ipi_read_trigger() & 0xFFFFull) != 0) {
        TEST_FAIL;
    }

    /* trigger sets bits */
    ipi_write_trigger(mask);
    if ((ipi_read_trigger() & 0xFFFFull) != mask) {
        TEST_FAIL;
    }

    /* trigger write of 0 is ignored */
    ipi_write_trigger(0);
    if ((ipi_read_trigger() & 0xFFFFull) != mask) {
        TEST_FAIL;
    }

    /* clear one bit */
    ipi_write_trigger_clear(0x0002ull);
    if ((ipi_read_trigger() & 0xFFFFull) != 0x0004ull) {
        TEST_FAIL;
    }

    /* clear write of 0 is ignored */
    ipi_write_trigger_clear(0);
    if ((ipi_read_trigger() & 0xFFFFull) != 0x0004ull) {
        TEST_FAIL;
    }

    /* clear remaining bit */
    ipi_write_trigger_clear(0x0004ull);
    if ((ipi_read_trigger() & 0xFFFFull) != 0x0ull) {
        TEST_FAIL;
    }

    TEST_PASS;
    return 0;
}
