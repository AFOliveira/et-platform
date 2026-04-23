/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk halify shim for test-compute-kernels/src/memset/memset.c.
 *
 * The kernel's entry_point takes `const Parameters*`, where Parameters
 * carries a destination buffer, fill value, element count, and shire
 * count. The kernel's own crt.S path receives this pointer from the
 * firmware launcher; under the gp-sdk runtime we control the entry and
 * synthesize a reasonable default here so the kernel runs end-to-end.
 *
 * Note on storage: the default Parameters + backing buffer live on the
 * shim function's stack rather than in .data/.bss. deviceGpSdkEntry's
 * init-sync fast-path is only skipped when both .bss and .data are empty
 * and there are no init arrays; on Erbium the sync path would execute
 * an amoadd.w, which the emulator lacks (no A extension on -march
 * rv64imfc). Keeping globals out of the shim preserves the fast-path.
 *-----------------------------------------------------------------------*/

#include <cstdint>
#include "entryPoint.h"

typedef struct {
    int* a;
    int value;
    int numElements;
    int numShires;
} Parameters;

extern "C" int entry_point(const Parameters*);

static int gpsdk_entry_shim_memset(void* /*args*/) {
    int buffer[64] __attribute__((aligned(64))) = {0};
    Parameters params = {
        /* .a           = */ buffer,
        /* .value       = */ static_cast<int>(0xdeadbeef),
        /* .numElements = */ 64,
        /* .numShires   = */ 1,
    };
    return entry_point(&params);
}

DECLARE_KERNEL_ENTRY_POINTS(gpsdk_entry_shim_memset, nullptr);
