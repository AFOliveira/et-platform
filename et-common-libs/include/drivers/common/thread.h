/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent thread0/1 disable access. Surface:
 *   thread_write_thread0_disable, thread_write_thread1_disable.
 */

#ifndef _DRIVERS_COMMON_THREAD_H_
#define _DRIVERS_COMMON_THREAD_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/thread_platform.h"
#else
#include "drivers/etsoc/thread_platform.h"
#endif

#endif /* _DRIVERS_COMMON_THREAD_H_ */
