/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 tensor/cache-op CSR backend.
 */

#ifndef ETSOC_API_TENSOR_PLATFORM_H_
#define ETSOC_API_TENSOR_PLATFORM_H_

#include <stdint.h>
#include <common/csr.h>
#include <etsoc_hal/inc/minion_csr.h>

/* ET-SoC1 minion_csr.h *_ADDRESS macros are csr numbers directly. */
#define _MCSR(addr) (addr)

static inline void platform_tensor_write_reduce(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_TENSOR_REDUCE_ADDRESS), val);
}

static inline uint64_t platform_tensor_read_error(void)
{
	return csr_read(_MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS));
}

static inline void platform_tensor_write_error(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_TENSOR_ERROR_ADDRESS), val);
}

static inline void platform_tensor_write_evict_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_EVICT_VA_ADDRESS), val);
}

static inline void platform_tensor_write_flush_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_FLUSH_VA_ADDRESS), val);
}

static inline void platform_tensor_write_prefetch_va(uint64_t val)
{
	csr_write(_MCSR(MINION_CSR_CO_PREFETCH_VA_ADDRESS), val);
}

#endif /* ETSOC_API_TENSOR_PLATFORM_H_ */
