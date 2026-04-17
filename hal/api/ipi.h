/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent IPI trigger API.
 */

#ifndef HAL_API_IPI_H_
#define HAL_API_IPI_H_

#include <stdint.h>
#include <api/ipi_platform.h>

static inline uint64_t ipi_read_trigger(void)
{
	return platform_ipi_read_trigger();
}

static inline void ipi_write_trigger(uint64_t val)
{
	platform_ipi_write_trigger(val);
}

static inline void ipi_write_trigger_clear(uint64_t val)
{
	platform_ipi_write_trigger_clear(val);
}

#endif /* HAL_API_IPI_H_ */
