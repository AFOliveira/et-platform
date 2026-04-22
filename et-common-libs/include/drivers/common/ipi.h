/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent IPI trigger / trigger-clear access. Backend
 * selected via ET_PLATFORM_ERBIUM. Surface:
 *   ipi_read_trigger, ipi_write_trigger, ipi_write_trigger_clear.
 */

#ifndef _DRIVERS_COMMON_IPI_H_
#define _DRIVERS_COMMON_IPI_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/ipi_platform.h"
#else
#include "drivers/etsoc/ipi_platform.h"
#endif

#endif /* _DRIVERS_COMMON_IPI_H_ */
