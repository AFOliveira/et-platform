/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium PLIC backend — RISC-V PLIC spec layout at ERBIUM_TOP_PLIC_BASE.
 *
 * Uses spec-level offset arithmetic rather than per-register hwinc macros,
 * since the PLIC register naming differs across platforms while the spec
 * layout is universal.
 */

#ifndef ERBIUM_API_PLIC_PLATFORM_H_
#define ERBIUM_API_PLIC_PLATFORM_H_

#include <stdint.h>
#include <erbium_hal/mmio.h>
#include <hwinc/top.h>

#define _PLIC_BASE ERBIUM_TOP_PLIC_BASE

/* RISC-V PLIC spec offsets */
#define _PLIC_PRIORITY_BASE   0x000000ul
#define _PLIC_PENDING_BASE    0x001000ul
#define _PLIC_ENABLE_BASE     0x002000ul
#define _PLIC_THRESHOLD_BASE  0x200000ul
#define _PLIC_CLAIM_BASE      0x200004ul

static inline uint32_t plic_read_priority(uint32_t src)
{
	return reg_read32(_PLIC_BASE + _PLIC_PRIORITY_BASE + src * 4);
}

static inline void plic_write_priority(uint32_t src, uint32_t val)
{
	reg_write32(_PLIC_BASE + _PLIC_PRIORITY_BASE + src * 4, val);
}

static inline uint32_t plic_read_pending(uint32_t word)
{
	return reg_read32(_PLIC_BASE + _PLIC_PENDING_BASE + word * 4);
}

static inline uint32_t plic_read_enable(uint32_t ctx, uint32_t word)
{
	return reg_read32(_PLIC_BASE + _PLIC_ENABLE_BASE + ctx * 0x80 + word * 4);
}

static inline void plic_write_enable(uint32_t ctx, uint32_t word, uint32_t val)
{
	reg_write32(_PLIC_BASE + _PLIC_ENABLE_BASE + ctx * 0x80 + word * 4, val);
}

static inline uint32_t plic_read_threshold(uint32_t ctx)
{
	return reg_read32(_PLIC_BASE + _PLIC_THRESHOLD_BASE + ctx * 0x1000);
}

static inline void plic_write_threshold(uint32_t ctx, uint32_t val)
{
	reg_write32(_PLIC_BASE + _PLIC_THRESHOLD_BASE + ctx * 0x1000, val);
}

static inline uint32_t plic_claim(uint32_t ctx)
{
	return reg_read32(_PLIC_BASE + _PLIC_CLAIM_BASE + ctx * 0x1000);
}

static inline void plic_complete(uint32_t ctx, uint32_t src)
{
	reg_write32(_PLIC_BASE + _PLIC_CLAIM_BASE + ctx * 0x1000, src);
}

#endif /* ERBIUM_API_PLIC_PLATFORM_H_ */
