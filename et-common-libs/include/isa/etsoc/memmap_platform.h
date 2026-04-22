/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 memory-region bases. Values from sw-sysemu/memory/etsoc1/
 * main_memory.h.
 *
 * HAL_MAIN_MEM_BASE points into the DRAM/OS sub-region, the conventional
 * place for test code/data. ET-SoC1 minions reset to 0x8000001000 (DRAM
 * + 0x1000), so code linked at HAL_MAIN_MEM_BASE is reachable from the
 * reset vector.
 *
 * HAL_SRAM_BASE points at PU/SRAM_LO (32KiB) in the IO region.
 *
 * HAL_BOOTROM_BASE is the SP ROM region. Minions on ET-SoC1 do not boot
 * from a dedicated bootrom like Erbium; this is exposed for consumers
 * that still want "some ROM region."
 *
 * HAL_ESR_BASE is the ESR region base (matches esr_platform.h encoder).
 */

#ifndef _ISA_ETSOC_MEMMAP_PLATFORM_H_
#define _ISA_ETSOC_MEMMAP_PLATFORM_H_

#define HAL_MAIN_MEM_BASE  0x0080000000ULL  /* DRAM region base */
#define HAL_SRAM_BASE      0x0020008000ULL  /* PU/SRAM_LO */
#define HAL_BOOTROM_BASE   0x0040000000ULL  /* SP/ROM */
#define HAL_ESR_BASE       0x0100000000ULL  /* ESR region */

#endif /* _ISA_ETSOC_MEMMAP_PLATFORM_H_ */
