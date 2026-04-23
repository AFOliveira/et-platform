/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk-halify probe: mirrors test-compute-kernels/src/log.
 * et_printf on hart 0 only.
 *-------------------------------------------------------------------------
 */

#include <common/etsoc/utils.h>
#include <isa/common/hart.h>
#include "entryPoint.h"

class KernelArguments;
int entryPoint_0(KernelArguments* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, nullptr);

int entryPoint_0([[maybe_unused]] KernelArguments* args) {
    if (get_hart_id() == 0) {
        et_printf("hello world from gp-sdk halify\n");
    }
    return 0;
}
