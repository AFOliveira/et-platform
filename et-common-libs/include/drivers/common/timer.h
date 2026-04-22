/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent mtime / mtimecmp access. Backend selected via
 * ET_PLATFORM_ERBIUM; both platforms expose the same function surface
 * (timer_read_mtime, timer_write_mtime, timer_read_mtimecmp,
 *  timer_write_mtimecmp, timer_read_mtime_local_target,
 *  timer_write_mtime_local_target).
 */

#ifndef _DRIVERS_COMMON_TIMER_H_
#define _DRIVERS_COMMON_TIMER_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/timer_platform.h"
#else
#include "drivers/etsoc/timer_platform.h"
#endif

#endif /* _DRIVERS_COMMON_TIMER_H_ */
