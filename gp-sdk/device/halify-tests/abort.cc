/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk-halify probe: mirrors test-compute-kernels/src/abort.
 * Builds on both ET-SoC1 (MachineMinion U-mode) and Erbium (Zephyr-M U-mode).
 *-------------------------------------------------------------------------
 */

#include <common/etsoc/utils.h>
#include "entryPoint.h"

class KernelArguments;
int entryPoint_0(KernelArguments* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, nullptr);

int entryPoint_0([[maybe_unused]] KernelArguments* args) {
    int temp = 0;
    temp++;
    et_abort();
    return 0;
}
