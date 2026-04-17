/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent fast local barrier (FLB) API.
 */

#ifndef HAL_API_BARRIER_H_
#define HAL_API_BARRIER_H_

#include <stdint.h>
#include <api/barrier_platform.h>

static inline uint64_t flb_exchange(uint64_t val)
{
	return platform_flb_exchange(val);
}

#endif /* HAL_API_BARRIER_H_ */
