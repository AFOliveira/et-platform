/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent RISC-V PLIC access. Both backends follow the
 * RISC-V PLIC spec layout; only the base address differs. Surface:
 *   plic_read_priority, plic_write_priority, plic_read_pending,
 *   plic_read_enable, plic_write_enable, plic_read_threshold,
 *   plic_write_threshold, plic_claim, plic_complete.
 */

#ifndef _DRIVERS_COMMON_PLIC_H_
#define _DRIVERS_COMMON_PLIC_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/plic_platform.h"
#else
#include "drivers/etsoc/plic_platform.h"
#endif

#endif /* _DRIVERS_COMMON_PLIC_H_ */
