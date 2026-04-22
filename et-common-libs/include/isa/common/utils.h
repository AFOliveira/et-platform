/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------
*/
/***********************************************************************/
/*! \file include/isa/common/utils.h
    \brief Platform-neutral utility macros and CSR-access inlines shared
    between ET-SoC1 and Erbium. Every CSR number used here was verified
    identical across the two platforms in the Phase 0 recon.

    ESR-backed helpers (fcc_send, flbarrier_set) live in isa/<platform>/fcc.h
    and isa/<platform>/flb.h respectively, since their bodies differ per
    platform.
*/
/***********************************************************************/
#ifndef _ISA_COMMON_UTILS_H_
#define _ISA_COMMON_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/*
 * General utils
 */

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

/*
 * Cache utils
 */

/*! \def CACHE_LINE_SIZE
    \brief Define for cache line size.
*/
#define CACHE_LINE_SIZE 64

/*! \def CACHE_STRUCT
    \brief Define a structure completely enclosed in exclusive cache lines.
*/
#define CACHE_STRUCT(_f)						\
    union {								\
	struct _f;							\
	char pad[((sizeof(struct _f) + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE * CACHE_LINE_SIZE)]; \
    } __attribute__((aligned(CACHE_LINE_SIZE)))


/*
 * RISC-V
 */

#define NOP   __asm__ __volatile__("nop\n");
#define FENCE __asm__ __volatile__("fence\n");
#define WFI   __asm__ __volatile__("wfi\n");

/*
 * CSR
 */

/*! \def WAIT_TENSOR_LOAD_0 */
#define WAIT_TENSOR_LOAD_0 __asm__ __volatile__("csrwi tensor_wait, 0\n" : :);
/*! \def WAIT_TENSOR_LOAD_1 */
#define WAIT_TENSOR_LOAD_1 __asm__ __volatile__("csrwi tensor_wait, 1\n" : :);
/*! \def WAIT_TENSOR_LOAD_L2_0 */
#define WAIT_TENSOR_LOAD_L2_0 __asm__ __volatile__("csrwi tensor_wait, 2\n" : :);
/*! \def WAIT_TENSOR_LOAD_L2_1 */
#define WAIT_TENSOR_LOAD_L2_1 __asm__ __volatile__("csrwi tensor_wait, 3\n" : :);
/*! \def WAIT_PREFETCH_0 */
#define WAIT_PREFETCH_0 __asm__ __volatile__("csrwi tensor_wait, 4\n" : :);
/*! \def WAIT_PREFETCH_1 */
#define WAIT_PREFETCH_1 __asm__ __volatile__("csrwi tensor_wait, 5\n" : :);
/*! \def WAIT_CACHEOPS */
#define WAIT_CACHEOPS __asm__ __volatile__("csrwi tensor_wait, 6\n" : :);
/*! \def WAIT_TENSOR_FMA */
#define WAIT_TENSOR_FMA __asm__ __volatile__("csrwi tensor_wait, 7\n" : :);
/*! \def WAIT_TENSOR_STORE */
#define WAIT_TENSOR_STORE __asm__ __volatile__("csrwi tensor_wait, 8\n" : :);
/*! \def WAIT_TENSOR_REDUCE */
#define WAIT_TENSOR_REDUCE __asm__ __volatile__("csrwi tensor_wait, 9\n" : :);
/*! \def WAIT_TENSOR_QUANT */
#define WAIT_TENSOR_QUANT __asm__ __volatile__("csrwi tensor_wait, 10\n" : :);

inline __attribute__((always_inline)) uint64_t mcache_control_get(void)
{
    uint64_t ret;
    __asm__ __volatile__("csrr  %0, mcache_control\n" : "=r"(ret));
    return ret;
}

inline __attribute__((always_inline)) void co_evict_sw(
    uint64_t use_tmask, uint64_t dst, uint64_t way, uint64_t set, uint64_t num_lines)
{
    uint64_t val = ((use_tmask & 1) << 63) | ((dst & 0x3) << 58) | ((set & 0xF) << 14) |
                   ((way & 0x3) << 6) | (num_lines & 0xF);
    __asm__ __volatile__("csrw 0x7f9, %0\n" : : "r"(val) : "memory");
}

inline __attribute__((always_inline)) void co_unlock_sw(uint64_t way, uint64_t set)
{
    uint64_t val = ((way & 3) << 55) | ((set & 0xF) << 6);
    __asm__ __volatile__("csrw 0x7ff, %0\n" : : "r"(val));
}

inline __attribute__((always_inline)) void tensorcooperation_write(uint64_t val)
{
    __asm__ __volatile__("csrw   0x804, %0\n" : : "r"(val));
}

inline __attribute__((always_inline)) void tensormask_write(uint64_t val)
{
    __asm__ __volatile__("csrw   0x805, %0\n" : : "r"(val));
}

inline __attribute__((always_inline)) void tensorerror_write(uint64_t val)
{
    __asm__ __volatile__("csrw   0x808, %0\n" : : "r"(val));
}

inline __attribute__((always_inline)) uint64_t flbarrier(uint64_t barrier_num, uint64_t match)
{
    uint64_t ret;
    uint64_t flb_arg = (match << 5) | (barrier_num & 0x1F);
    __asm__ __volatile__("csrrw  %0, 0x820, %1\n" : "=r"(ret) : "r"(flb_arg));
    return ret;
}

inline __attribute__((always_inline)) uint64_t flb(uint64_t barrier_num, uint64_t match)
{
    return flbarrier(barrier_num, match);
}

inline __attribute__((always_inline)) void fcc_consume(uint64_t fcc_reg)
{
    __asm__ __volatile__("csrw   fcc, %0\n" : : "r"(fcc_reg));
}

inline __attribute__((always_inline)) void fcc(uint64_t fcc_reg)
{
    fcc_consume(fcc_reg);
}

inline __attribute__((always_inline)) void stall(void)
{
    __asm__ __volatile__("csrw   stall, x0\n");
}

inline __attribute__((always_inline)) void tensorwait(uint64_t id)
{
    __asm__ __volatile__("csrw  tensor_wait, %0\n" : : "r"(id));
}

inline __attribute__((always_inline)) void portctrl0(uint64_t ctrl)
{
    __asm__ __volatile__("csrw   0x9cc, %0\n" : : "r"(ctrl) : "memory");
}

inline __attribute__((always_inline)) void portctrl1(uint64_t ctrl)
{
    __asm__ __volatile__("csrw   0x9cd, %0\n" : : "r"(ctrl) : "memory");
}

inline __attribute__((always_inline)) void portctrl2(uint64_t ctrl)
{
    __asm__ __volatile__("csrw   0x9ce, %0\n" : : "r"(ctrl) : "memory");
}

inline __attribute__((always_inline)) void portctrl3(uint64_t ctrl)
{
    __asm__ __volatile__("csrw   0x9cf, %0\n" : : "r"(ctrl) : "memory");
}

inline __attribute__((always_inline)) uint64_t fccnb(void)
{
    uint64_t ret;
    __asm__ __volatile__("csrr  %0, fccnb\n" : "=r"(ret));
    return ret;
}

inline __attribute__((always_inline)) void riscv_fence(void)
{
    __asm__ __volatile__("fence\n");
}

#ifdef __cplusplus
}
#endif

#endif /* _ISA_COMMON_UTILS_H_ */
