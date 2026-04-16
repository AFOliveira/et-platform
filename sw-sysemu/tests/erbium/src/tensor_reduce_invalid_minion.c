/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
* Test: TensorReduce with invalid sender/receiver minion ID
* Expected: tensor_error bit 9 is set
*
* On Erbium, valid minion IDs are 0..7 (EMU_NUM_MINIONS - 1).
* Writing a minion ID >= 8 should set tensor_error[9].
*/

#include "test.h"
#include <stdint.h>
#include <api/tensor.h>

#define TENSOR_CMD_SEND    0
#define TENSOR_REDUCE_SEND(minion_id)    (((minion_id) << 3) | TENSOR_CMD_SEND)
#define TENSOR_ERROR_INVALID_ID  (1 << 9)

int main() {
    uint64_t error;

    /* tensor_reduce CSR is only accessible from thread 0. */
    if (get_hart_id() % 2 != 0) {
        TEST_PASS;
        return 0;
    }

    /* Clear tensor_error */
    tensor_write_error(0);

    /* Verify tensor_error is cleared */
    error = tensor_read_error();
    if (error != 0) {
        TEST_FAIL;
    }

    /* Write tensor_reduce with invalid minion ID (8) */
    tensor_write_reduce(TENSOR_REDUCE_SEND(8));

    /* Read tensor_error and check bit 9 */
    error = tensor_read_error();
    if (error & TENSOR_ERROR_INVALID_ID) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
