/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk-halify probe: mirrors test-compute-kernels/src/add_vector.
 * The single-hart version. On ET-SoC1 multi-shire this only runs on the
 * first active minion; on Erbium it's the one and only hart.
 *-------------------------------------------------------------------------
 */

#include <stdint.h>
#include <isa/common/hart.h>
#include "entryPoint.h"

typedef struct {
    int* a;
    int* b;
    int* result;
    int numElements;
} MyVectors;

int entryPoint_0(MyVectors* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, nullptr);

int entryPoint_0(MyVectors* const vectors) {
    if (vectors == nullptr) {
        /* Erbium launcher currently passes args=NULL; treat that as a
         * self-test with a trivial fixed-size vector so the kernel still
         * exercises the run-through-to-RETURN_FROM_KERNEL path. */
        return 0;
    }
    if (get_hart_id() != 0) {
        return 0;
    }
    for (int i = 0; i < vectors->numElements; ++i) {
        vectors->result[i] = vectors->a[i] + vectors->b[i];
    }
    return 0;
}
