/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk-halify probe: mirrors test-compute-kernels/src/empty.
 * Does nothing, returns 0. The simplest possible gp-sdk kernel.
 *-------------------------------------------------------------------------
 */

#include "entryPoint.h"

class KernelArguments;
int entryPoint_0(KernelArguments* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, nullptr);

int entryPoint_0([[maybe_unused]] KernelArguments* args) {
    return 0;
}
