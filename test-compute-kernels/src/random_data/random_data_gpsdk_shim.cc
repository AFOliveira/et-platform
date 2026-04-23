/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk halify shim for test-compute-kernels/src/random_data/random_data.c.
 *
 * Kernel entry_point(const Parameters*) writes `length` bytes of LFSR
 * output to `data_ptr` from the first hart of each shire. Backing buffer
 * lives on the shim stack; see memset_gpsdk_shim.cc for why we keep
 * globals out of the shim.
 *-----------------------------------------------------------------------*/

#include <cstdint>
#include "entryPoint.h"

typedef struct {
    uint64_t* data_ptr;
    uint64_t length;
    uint64_t lfsr;
} Parameters;

extern "C" int64_t entry_point(const Parameters*);

static int gpsdk_entry_shim_random_data(void* /*args*/) {
    /* 256 uint64_t = 2 KiB: enough to exercise the inner loop without
     * bloating the kernel's stack usage. Length must be a multiple of 8
     * (bytes) -- expressed as bytes, 2048 qualifies. */
    uint64_t buffer[256] __attribute__((aligned(64))) = {0};
    Parameters params = {
        /* .data_ptr = */ buffer,
        /* .length   = */ sizeof(buffer),
        /* .lfsr     = */ 0xdeadbeefcafebabeULL,
    };
    return static_cast<int>(entry_point(&params));
}

DECLARE_KERNEL_ENTRY_POINTS(gpsdk_entry_shim_random_data, nullptr);
