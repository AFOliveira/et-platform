/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-Minion cache management HAL lib — stateless inline CSR operations.
 * OS-agnostic. Uses erbium_hal/csr.h for CSR primitives and
 * erbium_hal/csr_defs.h for CSR addresses and field macros.
 */

#ifndef ET_MINION_CACHE_H_
#define ET_MINION_CACHE_H_

#include <stdint.h>
#include <erbium_hal/csr.h>
#include <erbium_hal/csr_defs.h>

/* dest_level for CO_FLUSH_SW / CO_EVICT_SW: 1=L2, 2=L3, 3=DDR */
#define ET_CACHE_DEST_L2  1U
#define ET_CACHE_DEST_L3  2U
#define ET_CACHE_DEST_DDR 3U

/* Build a CO_FLUSH_SW / CO_EVICT_SW value: flush all sets/ways to dest */
#define ET_CO_SW_ALL(dest) \
	(MINION_CSR_FIELD_PREP(MINION_CSR_CO_FLUSH_SW_NUM_LINES, 0xFU) | \
	 MINION_CSR_FIELD_PREP(MINION_CSR_CO_FLUSH_SW_DEST_LEVEL, (dest)))

/**
 * Enable D-cache. HW state machine requires D1SPLIT (bit 0) before
 * SCPENABLE (bit 1). Direct 0→2 is silently dropped; must go 0→1→3.
 */
static inline void et_minion_dcache_enable(void)
{
	csr_set(MINION_CSR_MCACHE_CONTROL,
		MINION_CSR_MCACHE_CONTROL_D1SPLIT_MASK);
	csr_set(MINION_CSR_MCACHE_CONTROL,
		MINION_CSR_MCACHE_CONTROL_SCPENABLE_MASK);
}

static inline void et_minion_dcache_disable(void)
{
	csr_clear(MINION_CSR_MCACHE_CONTROL,
		  MINION_CSR_MCACHE_CONTROL_SCPENABLE_MASK);
}

static inline void et_minion_dcache_flush_all(void)
{
	csr_write(MINION_CSR_CO_FLUSH_SW, ET_CO_SW_ALL(ET_CACHE_DEST_L2));
}

static inline void et_minion_dcache_evict_all(void)
{
	csr_write(MINION_CSR_CO_EVICT_SW, ET_CO_SW_ALL(ET_CACHE_DEST_L2));
}

static inline void et_minion_icache_invd_all(void)
{
	csr_write(MINION_CSR_CACHEINVALIDATE,
		  MINION_CSR_CACHEINVALIDATE_I_CACHE_INVALIDATE_MASK);
}

static inline uint64_t et_minion_mcache_control_read(void)
{
	return csr_read(MINION_CSR_MCACHE_CONTROL);
}

#endif /* ET_MINION_CACHE_H_ */
