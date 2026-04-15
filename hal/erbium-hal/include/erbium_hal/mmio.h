/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Generic MMIO register access primitives for ET-SOC1 peripherals.
 * Stateless, header-only, OS-agnostic.
 */

#ifndef ETSOC_MMIO_H_
#define ETSOC_MMIO_H_

#include <stdint.h>
#include <stdbool.h>

static inline uint32_t etsoc_read32(uintptr_t addr)
{
	return *(volatile const uint32_t *)addr;
}

static inline void etsoc_write32(uintptr_t addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

static inline void etsoc_modify32(uintptr_t addr, uint32_t mask, uint32_t val)
{
	uint32_t r = etsoc_read32(addr);

	etsoc_write32(addr, (r & ~mask) | (val & mask));
}

static inline uint64_t etsoc_read64(uintptr_t addr)
{
	return *(volatile const uint64_t *)addr;
}

static inline void etsoc_write64(uintptr_t addr, uint64_t val)
{
	*(volatile uint64_t *)addr = val;
}

/**
 * Poll a register until (read & mask) == expected, or iteration limit reached.
 * Returns 0 on success, -1 on timeout.
 */
static inline int etsoc_wait_for_bits(uintptr_t addr, uint32_t mask,
				      uint32_t expected, uint32_t max_iter)
{
	while (max_iter--) {
		if ((etsoc_read32(addr) & mask) == expected) {
			return 0;
		}
	}
	return -1;
}

static inline int etsoc_wait_bit_set(uintptr_t addr, uint32_t bit,
				     uint32_t max_iter)
{
	return etsoc_wait_for_bits(addr, bit, bit, max_iter);
}

static inline int etsoc_wait_bit_clear(uintptr_t addr, uint32_t bit,
				       uint32_t max_iter)
{
	return etsoc_wait_for_bits(addr, bit, 0, max_iter);
}

#endif /* ETSOC_MMIO_H_ */
