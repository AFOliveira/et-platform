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
#include <hwinc/minion_csr.h>

#define TENSOR_CMD_SEND    0
#define TENSOR_REDUCE_SEND(minion_id)    (((minion_id) << 3) | TENSOR_CMD_SEND)
#define TENSOR_ERROR_INVALID_ID \
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

    /* Write tensor_reduce with invalid minion ID (8) */
    hal_tensor_write_reduce(TENSOR_REDUCE_SEND(8));

    /* Read tensor_error and check bit 9 */
    error = hal_tensor_read_error();
    if (error & TENSOR_ERROR_INVALID_ID) {
        TEST_PASS;
    }

    TEST_FAIL;
    return 0;
}
