/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * RISC-V CSR access primitives.
 */

#ifndef _ISA_COMMON_CSR_H_
#define _ISA_COMMON_CSR_H_

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)

#define csr_read(csr)                                         \
({                                                            \
    register unsigned long __rv;                              \
    __asm__ volatile ("csrr %0, " STRINGIFY(csr)              \
                      : "=r" (__rv));                         \
    __rv;                                                     \
})

#define csr_write(csr, val)                                   \
    do {                                                      \
        unsigned long __wv = (unsigned long)(val);            \
        __asm__ volatile ("csrw " STRINGIFY(csr) ", %0"       \
                          :                                   \
                          : "rK" (__wv)                       \
                          : "memory");                        \
    } while (0)

#define csr_set(csr, val)                                     \
    do {                                                      \
        unsigned long __sv = (unsigned long)(val);            \
        __asm__ volatile ("csrs " STRINGIFY(csr) ", %0"       \
                          :                                   \
                          : "rK" (__sv)                       \
                          : "memory");                        \
    } while (0)

#define csr_clear(csr, val)                                   \
    do {                                                      \
        unsigned long __cv = (unsigned long)(val);            \
        __asm__ volatile ("csrc " STRINGIFY(csr) ", %0"       \
                          :                                   \
                          : "rK" (__cv)                       \
                          : "memory");                        \
    } while (0)

#endif /* _ISA_COMMON_CSR_H_ */
