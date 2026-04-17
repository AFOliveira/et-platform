/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent memory protection (MPROT) API.
 */

#ifndef HAL_API_MPROT_H_
#define HAL_API_MPROT_H_

#include <stdint.h>
#include <api/mprot_platform.h>

static inline uint64_t mprot_read(void)
{
	return platform_mprot_read();
}

static inline void mprot_write(uint64_t val)
{
	platform_mprot_write(val);
}

#endif /* HAL_API_MPROT_H_ */
