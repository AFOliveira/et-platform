/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium tensor/cache-op CSR backend.
 *
 * minion_csr.h encodes byte_offset = csr_number * 8, so we divide
 * the _ADDRESS by 8 to recover the CSR number for inline asm.
 */

#ifndef ERBIUM_API_TENSOR_PLATFORM_H_
#define ERBIUM_API_TENSOR_PLATFORM_H_

#include <stdint.h>
#include <erbium_hal/csr.h>
#include <hwinc/minion_csr.h>

/* Convert minion_csr byte address to CSR number */
#define _MCSR(addr) ((addr) / 8)

static inline void tensor_write_reduce(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_TENSOR_REDUCE_ADDRESS), val);
}

static inline uint64_t tensor_read_error(void)
{
	return csr_read(_MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS));
}

static inline void tensor_write_error(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS), val);
}

static inline void tensor_write_evict_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_EVICT_VA_ADDRESS), val);
}

static inline void tensor_write_flush_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_FLUSH_VA_ADDRESS), val);
}

static inline void tensor_write_prefetch_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_PREFETCH_VA_ADDRESS), val);
}

#endif /* ERBIUM_API_TENSOR_PLATFORM_H_ */
