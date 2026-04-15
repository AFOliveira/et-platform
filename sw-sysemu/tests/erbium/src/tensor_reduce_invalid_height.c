/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: TensorReduce with invalid tree_depth (height)
* Expected: tensor_error bit 9 is set
*
* On Erbium, valid tree_depth is 0..2 (max 2^(2+1) = 8 minions).
* Writing a height > 2 should set tensor_error[9].
*
* HAL-ified variant: uses csr_read/csr_write from the Erbium HAL
* instead of hand-rolled inline asm, and references CSR numbers via
* MINION_CSR_*_ADDRESS from the generated register defs. The synthetic
* minion_csr.rdl encodes byte_offset = csr_number * 8, so divide by 8
* to recover the CSR number the assembler expects.
*/

#include "test.h"
#include <erbium_hal/csr.h>
#include <hwinc/minion_csr.h>

/* Recover CSR number from the synthetic byte offset */
#define MCSR(addr) ((addr) / 8)

/* TensorReduce command encoding:
 * bits [1:0]  = command (0=send, 1=receive, 2=broadcast, 3=reduce)
 * bits [6:3]  = height/tree_depth (for broadcast/reduce)
 */
#define TENSOR_CMD_BROADCAST 2

/* Build tensor_reduce value for broadcast/reduce with given height */
#define TENSOR_REDUCE_BROADCAST(height) (((height) << 3) | TENSOR_CMD_BROADCAST)

/* tensor_error bit 9 indicates invalid tree_depth */
#define TENSOR_ERROR_INVALID_HEIGHT (1 << 9)

int main() {
    uint64_t error;

    /* tensor_reduce CSR is only accessible from thread 0.
     * Other threads should exit immediately. */
    if (get_hart_id() % 2 != 0) {
        TEST_PASS;
        return 0;
    }

    /* Clear tensor_error */
    csr_write(MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS), 0);

    /* Verify tensor_error is cleared */
    error = csr_read(MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS));
    if (error != 0) {
        TEST_FAIL;
    }

    /* Write tensor_reduce broadcast with invalid height (3) */
    csr_write(MCSR(MINION_CSR_TENSOR_REDUCE_ADDRESS),
              TENSOR_REDUCE_BROADCAST(3));

    /* Read tensor_error and check bit 9 */
    error = csr_read(MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS));
    if (error & TENSOR_ERROR_INVALID_HEIGHT) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
