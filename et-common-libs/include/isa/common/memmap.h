/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent memory-region bases. Each platform backend
 * supplies the HAL_* values in isa/<platform>/memmap_platform.h,
 * selected here via ET_PLATFORM_ERBIUM.
 */

#ifndef _ISA_COMMON_MEMMAP_H_
#define _ISA_COMMON_MEMMAP_H_

#ifdef ET_PLATFORM_ERBIUM
#include "isa/erbium/memmap_platform.h"
#else
#include "isa/etsoc/memmap_platform.h"
#endif

#endif /* _ISA_COMMON_MEMMAP_H_ */
