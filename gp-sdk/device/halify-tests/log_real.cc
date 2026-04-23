/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk-halify probe: exercises et_printf unconditionally (every hart),
 * with no get_hart_id()==X guard. On ETSoC1 this produces trace output.
 * On Erbium, where the trace backend is a no-op (see
 * et-common-libs/include/common/etsoc/utils.h), it must simply fall
 * through to RETURN_FROM_KERNEL without a load-access-fault on
 * CM_UMODE_TRACE_CB.
 *-------------------------------------------------------------------------
 */

#include <common/etsoc/utils.h>
#include "entryPoint.h"

class KernelArguments;
int entryPoint_0(KernelArguments* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, nullptr);

int entryPoint_0([[maybe_unused]] KernelArguments* args) {
    et_printf("hello world %d\n", 42);
    return 0;
}
