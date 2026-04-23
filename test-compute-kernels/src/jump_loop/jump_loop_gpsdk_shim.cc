/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk halify shim for test-compute-kernels/src/jump_loop/jump_loop.c.
 *
 * Kernel entry_point(const Parameters*) just runs an inline assembly
 * count-up loop. We pick a small num_iters so the kernel finishes
 * promptly under the emulator's cycle budget.
 *-----------------------------------------------------------------------*/

#include <cstdint>
#include "entryPoint.h"

typedef struct {
    uint64_t num_iters;
} Parameters;

extern "C" int entry_point(const Parameters*);

static int gpsdk_entry_shim_jump_loop(void* /*args*/) {
    Parameters params = {
        /* .num_iters = */ 1024,
    };
    return entry_point(&params);
}

DECLARE_KERNEL_ENTRY_POINTS(gpsdk_entry_shim_jump_loop, nullptr);
