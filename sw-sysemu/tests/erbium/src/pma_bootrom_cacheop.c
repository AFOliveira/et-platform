/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: On BootROM, only PrefetchVA is allowed among VA cache operations
*
* Expected:
* - EvictVA / FlushVA / LockVA / UnlockVA set TensorError[7]
* - PrefetchVA does not set TensorError[7]
*/

#include "test.h"

#define BOOTROM_BASE    0x0200A000ull

/* CSR addresses */
#define CSR_TENSOR_ERROR  0x808
#define CSR_EVICT_VA      0x89f
#define CSR_FLUSH_VA      0x8bf
#define CSR_PREFETCH_VA   0x81f
#define CSR_LOCK_VA       0x8df
#define CSR_UNLOCK_VA     0x8ff

/* VA op values: dest=1 (L2), addr=BOOTROM_BASE, count=0 (1 line) */
#define EVICT_VA_BOOTROM    ((1ull << 58) | BOOTROM_BASE)
#define FLUSH_VA_BOOTROM    ((1ull << 58) | BOOTROM_BASE)

/* PrefetchVA value: dest=0 (L1), addr=BOOTROM_BASE, count=0 (1 line) */
#define PREFETCH_VA_BOOTROM  (BOOTROM_BASE)

#define LOCK_VA_BOOTROM      (BOOTROM_BASE)
#define UNLOCK_VA_BOOTROM    (BOOTROM_BASE)

#define TENSOR_ERROR_PMA  (1 << 7)

static inline void clear_tensor_error(void) {
    asm volatile("csrw %0, zero" :: "i"(CSR_TENSOR_ERROR));
}

static inline uint64_t read_tensor_error(void) {
    uint64_t val;
    asm volatile("csrr %0, %1" : "=r"(val) : "i"(CSR_TENSOR_ERROR));
    return val;
}

int main() {
    /* EvictVA on BootROM - should fail */
    clear_tensor_error();
    asm volatile("csrw %0, %1" :: "i"(CSR_EVICT_VA), "r"(EVICT_VA_BOOTROM));
    if (!(read_tensor_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    /* FlushVA on BootROM - should fail */
    clear_tensor_error();
    asm volatile("csrw %0, %1" :: "i"(CSR_FLUSH_VA), "r"(FLUSH_VA_BOOTROM));
    if (!(read_tensor_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    /* LockVA on BootROM - should fail */
    clear_tensor_error();
    asm volatile("csrw %0, %1" :: "i"(CSR_LOCK_VA), "r"(LOCK_VA_BOOTROM));
    if (!(read_tensor_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    /* UnlockVA on BootROM - should fail */
    clear_tensor_error();
    asm volatile("csrw %0, %1" :: "i"(CSR_UNLOCK_VA), "r"(UNLOCK_VA_BOOTROM));
    if (!(read_tensor_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    /* PrefetchVA on BootROM - should succeed */
    clear_tensor_error();
    asm volatile("csrw %0, %1" :: "i"(CSR_PREFETCH_VA), "r"(PREFETCH_VA_BOOTROM));
    if (read_tensor_error() & TENSOR_ERROR_PMA)
        TEST_FAIL;

    TEST_PASS;
    return 0;
}
