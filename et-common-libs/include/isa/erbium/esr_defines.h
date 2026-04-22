/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file  isa/erbium/esr_defines.h
 *  \brief Erbium ESR address encoding.
 *
 *  Erbium exposes its External System Registers at
 *  ERBIUM_TOP_CPU_REGISTERS_BASE (0x80000000) with the following layout:
 *
 *      bit [30:24]  shire id
 *      bit [23:22]  privilege (PP)
 *      bit [21:0]   sub-region + register offset
 *
 *  Per-register offsets (USER_CPU / MACHINE_CPU / MACHINE_NEIGH / ...)
 *  come from hal/platform/erbium/include/hwinc/esr.h.
 *
 *  Unlike ET-SoC1, Erbium has no shire-wide ESR region for things like
 *  FLBs and FCC credits — those live in the per-CPU USER_CPU sub-region
 *  of the caller's own ESR window. isa/erbium/flb.h and isa/erbium/fcc.h
 *  encapsulate that addressing.
 */

#ifndef _ISA_ERBIUM_ESR_DEFINES_H_
#define _ISA_ERBIUM_ESR_DEFINES_H_

#include <stdint.h>

#define ERBIUM_ESR_BASE          0x80000000ul
#define ERBIUM_ESR_PP_SHIFT      22
#define ERBIUM_ESR_SHIRE_SHIFT   24

/* Encode an explicit (shire, pp, offset) ESR address. */
#define ERBIUM_ESR(shire, pp, offset)                                          \
    (ERBIUM_ESR_BASE                                                           \
     | ((uintptr_t)(shire) << ERBIUM_ESR_SHIRE_SHIFT)                          \
     | ((uintptr_t)(pp) << ERBIUM_ESR_PP_SHIFT)                                \
     | (uintptr_t)(offset))

/* Base of the caller's own ESR window. Hardware resolves the shire and
 * privilege bits from the issuing hart's context. */
#define ERBIUM_ESR_SELF          ERBIUM_ESR_BASE

#endif /* _ISA_ERBIUM_ESR_DEFINES_H_ */
