/*-------------------------------------------------------------------------
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * gp-sdk halify shim for test-compute-kernels/src/crc32/crc32.c.
 *
 * Kernel entry_point(const Parameters*) computes a CRC32 over a
 * 4-byte-aligned buffer and stores the result at *crc_ptr. Backing
 * buffer + output word live on the shim stack; see memset_gpsdk_shim.cc
 * for why we keep globals out of the shim.
 *-----------------------------------------------------------------------*/

#include <cstdint>
#include "entryPoint.h"

typedef struct {
    void* data_ptr;
    uint64_t length;
    uint32_t* crc_ptr;
} Parameters;

extern "C" int64_t entry_point(const Parameters*);

static int gpsdk_entry_shim_crc32(void* /*args*/) {
    /* 1 KiB of sample data. Pre-seeded with a deterministic pattern so
     * the CRC is reproducible across runs (not that we check it here --
     * the test just verifies the kernel runs to completion). */
    uint8_t buffer[1024] __attribute__((aligned(64))) = {0};
    for (int i = 0; i < 1024; ++i) {
        buffer[i] = static_cast<uint8_t>(i & 0xff);
    }
    uint32_t crc_out = 0;
    Parameters params = {
        /* .data_ptr = */ buffer,
        /* .length   = */ sizeof(buffer),
        /* .crc_ptr  = */ &crc_out,
    };
    return static_cast<int>(entry_point(&params));
}

DECLARE_KERNEL_ENTRY_POINTS(gpsdk_entry_shim_crc32, nullptr);
