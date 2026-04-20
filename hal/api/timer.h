/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent timer access API.
 */

#ifndef HAL_API_TIMER_H_
#define HAL_API_TIMER_H_

#include <stdint.h>
#include <api/timer_platform.h>

static inline uint64_t hal_timer_read_mtime(void)
{
	return platform_timer_read_mtime();
}

static inline void hal_timer_write_mtime(uint64_t val)
{
	platform_timer_write_mtime(val);
}

static inline uint64_t hal_timer_read_mtimecmp(void)
{
	return platform_timer_read_mtimecmp();
}

static inline void hal_timer_write_mtimecmp(uint64_t val)
{
	platform_timer_write_mtimecmp(val);
}

static inline uint64_t hal_timer_read_mtime_local_target(void)
{
	return platform_timer_read_mtime_local_target();
}

static inline void hal_timer_write_mtime_local_target(uint64_t val)
{
	platform_timer_write_mtime_local_target(val);
}

#endif /* HAL_API_TIMER_H_ */
