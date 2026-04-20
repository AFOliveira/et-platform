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
#include "minion_csr_compat.h"
#include <common/memmap.h>

#define MRAM_BASE    HAL_MAIN_MEM_BASE
#define SRAM_BASE    HAL_SRAM_BASE
#define BOOTROM_BASE HAL_BOOTROM_BASE

#define TENSOR_ERROR_PMA  MINION_CSR_TENSOR_ERROR_MEM_FAULT_FIELD_MASK

/* EvictVA/FlushVA need dest >= 1 to reach the PMA check (dest=0 is L1-only, skipped) */
#define EVICT_VA(addr)    (MINION_CSR_CO_EVICT_VA_DEST_LEVEL_SET(1ull) | (addr))
#define FLUSH_VA(addr)    (MINION_CSR_CO_FLUSH_VA_DEST_LEVEL_SET(1ull) | (addr))
#define PREFETCH_VA(addr) (addr)

/* Expect CacheOps to succeed (no PMA error) */
static void check_cacheops_ok(uint64_t addr) {
    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_EVICT_VA, EVICT_VA(addr));
    if (minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA)
        TEST_FAIL;

    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_FLUSH_VA, FLUSH_VA(addr));
    if (minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA)
        TEST_FAIL;

    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_PREFETCH_VA, PREFETCH_VA(addr));
    if (minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA)
        TEST_FAIL;
}

/* Expect CacheOps to fail (PMA error) */
static void check_cacheops_fail(uint64_t addr) {
    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_EVICT_VA, EVICT_VA(addr));
    if (!(minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA))
        TEST_FAIL;

    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_FLUSH_VA, FLUSH_VA(addr));
    if (!(minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA))
        TEST_FAIL;

    minion_csr_write(TENSOR_ERROR, 0);
    minion_csr_write(CO_PREFETCH_VA, PREFETCH_VA(addr));
    if (!(minion_csr_read(TENSOR_ERROR) & TENSOR_ERROR_PMA))
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
