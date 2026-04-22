/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent memory-protection register access. Surface:
 *   mprot_read, mprot_write.
 */

#ifndef _DRIVERS_COMMON_MPROT_H_
#define _DRIVERS_COMMON_MPROT_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/mprot_platform.h"
#else
#include "drivers/etsoc/mprot_platform.h"
#endif

#endif /* _DRIVERS_COMMON_MPROT_H_ */
