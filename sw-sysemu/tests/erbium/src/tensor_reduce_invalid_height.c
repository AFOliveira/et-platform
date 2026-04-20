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
*/

#include "test.h"
#include <api/tensor.h>
#include <hwinc/minion_csr.h>

#define TENSOR_CMD_BROADCAST 2
#define TENSOR_REDUCE_BROADCAST(height) \
    (MINION_CSR_TENSOR_REDUCE_TREE_DEPTH_SET(height) | \
     MINION_CSR_TENSOR_REDUCE_TENSOR_OP_ENCODING_SET(TENSOR_CMD_BROADCAST))
#define TENSOR_ERROR_INVALID_HEIGHT \
    MINION_CSR_TENSOR_ERROR_ILLEGAL_TENSOR_SEND_RCV_FIELD_MASK

int main() {
    uint64_t error;

    /* tensor_reduce CSR is only accessible from thread 0. */
    if (get_hart_id() % 2 != 0) {
        TEST_PASS;
        return 0;
    }

    /* Clear tensor_error */
    hal_tensor_write_error(0);

    /* Verify tensor_error is cleared */
    error = hal_tensor_read_error();
    if (error != 0) {
        TEST_FAIL;
    }

    /* Write tensor_reduce broadcast with invalid height (3) */
    hal_tensor_write_reduce(TENSOR_REDUCE_BROADCAST(3));

    /* Read tensor_error and check bit 9 */
    error = hal_tensor_read_error();
    if (error & TENSOR_ERROR_INVALID_HEIGHT) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
