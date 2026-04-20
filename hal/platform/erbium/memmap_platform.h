/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium memory-region bases.
 */

#ifndef ERBIUM_MEMMAP_PLATFORM_H_
#define ERBIUM_MEMMAP_PLATFORM_H_

#include <hwinc/top.h>

#define HAL_MAIN_MEM_BASE  ERBIUM_TOP_MRAM_BASE
#define HAL_SRAM_BASE      ERBIUM_TOP_SRAM_BASE
#define HAL_BOOTROM_BASE   ERBIUM_TOP_BOOTROM_BASE
#define HAL_ESR_BASE       ERBIUM_TOP_CPU_REGISTERS_BASE

#endif /* ERBIUM_MEMMAP_PLATFORM_H_ */
