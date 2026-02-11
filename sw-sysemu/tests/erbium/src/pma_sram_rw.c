/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Basic SRAM read/write access
*/

#include "test.h"
#include <stdint.h>

/* SRAM region: 0x40005000 - 0x400057FF (2KB) */
#define SRAM_BASE 0x40005000ull
#define TEST_PATTERN 0xCAFEFEEDDEADBEEFull

int main() {
    volatile uint64_t *sram = (volatile uint64_t *)SRAM_BASE;

    /* Write test pattern */
    *sram = TEST_PATTERN;

    /* Read back and verify */
    if (*sram == TEST_PATTERN) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
