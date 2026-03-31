/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Store to MRAM boot protocol region triggers store access fault
*
* The first 256 bytes of MRAM (0x40000000-0x400000FF) are reserved for
* the boot protocol and write-protected from CPU stores.
* Expected: Store access fault (cause=7), test PASSes via trap handler
*/

#include "test.h"
#include "trap.h"

#define MRAM_BOOT_REGION 0x40000020ull  /* Payload SP offset */

int main() {
    expect_exception(CAUSE_STORE_ACCESS_FAULT);

    volatile uint64_t *boot_region = (volatile uint64_t *)MRAM_BOOT_REGION;
    *boot_region = 0xDEADBEEF;

    TEST_FAIL;
    return 0;
}
