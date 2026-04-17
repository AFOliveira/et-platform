/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent hart thread-disable API.
 */

#ifndef HAL_API_THREAD_H_
#define HAL_API_THREAD_H_

#include <stdint.h>
#include <api/thread_platform.h>

static inline void thread_write_thread0_disable(uint64_t val)
{
	platform_thread_write_thread0_disable(val);
}

static inline void thread_write_thread1_disable(uint64_t val)
{
	platform_thread_write_thread1_disable(val);
}

#endif /* HAL_API_THREAD_H_ */
