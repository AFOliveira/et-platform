/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Test-local shim that picks the right Minion CSR hwinc header per
 * platform and provides csrr/csrw helpers keyed by CSR name. Not part
 * of the HAL — Minion CSRs are a custom ISA extension, not an SoC
 * peripheral, so they live outside hal/.
 *
 * Both platforms' generated hwinc expose MINION_CSR_<NAME>_BYTE_ADDRESS
 * as the byte offset of the register. The csr number is byte / 8.
 */

#ifndef ERBIUM_TESTS_MINION_CSR_COMPAT_H
#define ERBIUM_TESTS_MINION_CSR_COMPAT_H

#include <stdint.h>

#if defined(PLATFORM_IS_ETSOC)
#  include <etsoc_hal/inc/minion_csr.h>
#else
#  include <hwinc/minion_csr.h>
#endif

#define MCSR(name) ((MINION_CSR_##name##_BYTE_ADDRESS) / 8)

#define minion_csr_read(name)                                         \
({                                                                    \
	uint64_t __rv;                                                \
	__asm__ volatile ("csrr %0, %1"                               \
			  : "=r"(__rv)                                \
			  : "i"(MCSR(name)));                         \
	__rv;                                                         \
})

#define minion_csr_write(name, val)                                   \
	do {                                                          \
		unsigned long __wv = (unsigned long)(val);            \
		__asm__ volatile ("csrw %0, %1"                       \
				  :                                   \
				  : "i"(MCSR(name)), "r"(__wv)        \
				  : "memory");                        \
	} while (0)

#define minion_csr_swap(name, val)                                    \
({                                                                    \
	uint64_t __rv;                                                \
	unsigned long __wv = (unsigned long)(val);                    \
	__asm__ volatile ("csrrw %0, %1, %2"                          \
			  : "=r"(__rv)                                \
			  : "i"(MCSR(name)), "r"(__wv));              \
	__rv;                                                         \
})

#endif /* ERBIUM_TESTS_MINION_CSR_COMPAT_H */
