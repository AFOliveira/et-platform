/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: CacheOps on cacheable memory regions
*
* CacheOps are classified as writes in the Erbium PMA. BootROM is
* read-only, so all VA cache operations must set TensorError[7] there.
* On writable regions (MRAM, SRAM) they must succeed.
*/

#include "test.h"
#include <hwinc/top.h>
#include <api/tensor.h>

#define MRAM_BASE    ERBIUM_TOP_MRAM_BASE
#define SRAM_BASE    ERBIUM_TOP_SRAM_BASE
#define BOOTROM_BASE ERBIUM_TOP_BOOTROM_BASE

#define TENSOR_ERROR_PMA  (1 << 7)

/* EvictVA/FlushVA need dest >= 1 to reach the PMA check (dest=0 is L1-only, skipped) */
#define EVICT_VA(addr)    ((1ull << 58) | (addr))
#define FLUSH_VA(addr)    ((1ull << 58) | (addr))
#define PREFETCH_VA(addr) (addr)

/* Expect CacheOps to succeed (no PMA error) */
static void check_cacheops_ok(uint64_t addr) {
    tensor_write_error(0);
    tensor_write_evict_va(EVICT_VA(addr));
    if (tensor_read_error() & TENSOR_ERROR_PMA)
        TEST_FAIL;

    tensor_write_error(0);
    tensor_write_flush_va(FLUSH_VA(addr));
    if (tensor_read_error() & TENSOR_ERROR_PMA)
        TEST_FAIL;

    tensor_write_error(0);
    tensor_write_prefetch_va(PREFETCH_VA(addr));
    if (tensor_read_error() & TENSOR_ERROR_PMA)
        TEST_FAIL;
}

/* Expect CacheOps to fail (PMA error) */
static void check_cacheops_fail(uint64_t addr) {
    tensor_write_error(0);
    tensor_write_evict_va(EVICT_VA(addr));
    if (!(tensor_read_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    tensor_write_error(0);
    tensor_write_flush_va(FLUSH_VA(addr));
    if (!(tensor_read_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;

    tensor_write_error(0);
    tensor_write_prefetch_va(PREFETCH_VA(addr));
    if (!(tensor_read_error() & TENSOR_ERROR_PMA))
        TEST_FAIL;
}

int main() {
    /* MRAM: CacheOps should succeed */
    check_cacheops_ok(MRAM_BASE);

    /* SRAM: CacheOps should succeed */
    check_cacheops_ok(SRAM_BASE);

    /* BootROM: CacheOps must fail (read-only region) */
    check_cacheops_fail(BOOTROM_BASE);

    TEST_PASS;
    return 0;
}
