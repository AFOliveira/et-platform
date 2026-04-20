/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent PLIC access API.
 */

#ifndef HAL_API_PLIC_H_
#define HAL_API_PLIC_H_

#include <stdint.h>
#include <api/plic_platform.h>

static inline uint32_t hal_plic_read_priority(uint32_t src)
{
	return platform_plic_read_priority(src);
}

static inline void hal_plic_write_priority(uint32_t src, uint32_t val)
{
	platform_plic_write_priority(src, val);
}

static inline uint32_t hal_plic_read_pending(uint32_t word)
{
	return platform_plic_read_pending(word);
}

static inline uint32_t hal_plic_read_enable(uint32_t ctx, uint32_t word)
{
	return platform_plic_read_enable(ctx, word);
}

static inline void hal_plic_write_enable(uint32_t ctx, uint32_t word, uint32_t val)
{
	platform_plic_write_enable(ctx, word, val);
}

static inline uint32_t hal_plic_read_threshold(uint32_t ctx)
{
	return platform_plic_read_threshold(ctx);
}

static inline void hal_plic_write_threshold(uint32_t ctx, uint32_t val)
{
	platform_plic_write_threshold(ctx, val);
}

static inline uint32_t hal_plic_claim(uint32_t ctx)
{
	return platform_plic_claim(ctx);
}

static inline void hal_plic_complete(uint32_t ctx, uint32_t src)
{
	platform_plic_complete(ctx, src);
}

#endif /* HAL_API_PLIC_H_ */
