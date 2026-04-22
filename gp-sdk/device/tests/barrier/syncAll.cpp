
/*-------------------------------------------------------------------------
 * Copyright (c) 2025 Ainekko, Co.
 * SPDX-License-Identifier: Apache-2.0
 *-------------------------------------------------------------------------
 */

#include <common/etsoc/utils.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/hart.h>
#include <isa/etsoc/tensors.h>
#include <isa/etsoc/utils.h>

#include "entryPoint.h"
#include "sync.h"
#include "CommonCode.h"


class KernelArguments;
int entryPoint_0(KernelArguments* args);
DECLARE_KERNEL_ENTRY_POINTS(entryPoint_0, entryPoint_0);

__attribute__((noinline)) int entryPoint_0([[maybe_unused]] KernelArguments* args) {
  hart::barrier();
  et_printf("Success\n");
  return 0;
}

