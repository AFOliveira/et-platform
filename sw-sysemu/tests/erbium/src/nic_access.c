/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: NIC (NI-700) configuration space is accessible
 * Expected: PASS
 *
 * The NIC at 0xFE000000 is a RAZ/WI stub in the emulator.
 * Verify that reads return 0 and writes don't fault.
 */

#include "test.h"
#include <stdint.h>

#define NIC_BASE 0xFE000000ull

int main()
{
    volatile uint32_t *nic = (volatile uint32_t *)NIC_BASE;
    uint32_t val;

    /* Read from base — should return 0, not fault */
    val = nic[0];
    if (val != 0)
        TEST_FAIL;

    /* Write should not fault */
    nic[0] = 0xDEADBEEF;

    /* Read back — DenseRegion stores it, so expect the written value */
    val = nic[0];
    if (val != 0xDEADBEEF)
        TEST_FAIL;

    /* Read from further offset — should return 0 */
    val = nic[0x100];
    if (val != 0)
        TEST_FAIL;

    TEST_PASS;
    return 0;
}
