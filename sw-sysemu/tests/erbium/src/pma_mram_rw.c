/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: Basic MRAM read/write access
*/

#include "test.h"
#include <stdint.h>
#include <hwinc/top.h>

#define MRAM_BASE ERBIUM_TOP_MRAM_BASE
#define TEST_PATTERN 0xDEADBEEFCAFEFEEDull

int main() {
    volatile uint64_t *mram = (volatile uint64_t *)MRAM_BASE;

    *mram = TEST_PATTERN;

    if (*mram == TEST_PATTERN) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
