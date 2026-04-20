/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent tensor/cache-op CSR access API.
 */

#ifndef HAL_API_TENSOR_H_
#define HAL_API_TENSOR_H_

#include <stdint.h>
#include <api/tensor_platform.h>

static inline void hal_tensor_write_reduce(uint64_t val)
{
	platform_tensor_write_reduce(val);
}

static inline uint64_t hal_tensor_read_error(void)
{
	return platform_tensor_read_error();
}

static inline void hal_tensor_write_error(uint64_t val)
{
	platform_tensor_write_error(val);
}

static inline void hal_tensor_write_evict_va(uint64_t val)
{
	platform_tensor_write_evict_va(val);
}

static inline void hal_tensor_write_flush_va(uint64_t val)
{
	platform_tensor_write_flush_va(val);
}

static inline void hal_tensor_write_prefetch_va(uint64_t val)
{
	platform_tensor_write_prefetch_va(val);
}

#endif /* HAL_API_TENSOR_H_ */
