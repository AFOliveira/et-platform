/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------
*/
/***********************************************************************/
/*! \file include/isa/etsoc/utils.h
    \brief ET-SoC1 slice of the shared ISA utils header.

    The platform-neutral bulk (CSR-access inlines, tensor_wait macros,
    misc helpers) lives in isa/common/utils.h and is pulled in below.
    This file adds the two ESR-backed helpers (fcc_send, flbarrier_set)
    that differ between ET-SoC1 and Erbium.

    Kept at this path so existing callers using <isa/etsoc/utils.h>
    continue to compile unchanged.
*/
/***********************************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "isa/common/utils.h"
#include "isa/etsoc/esr_defines.h"

/*! \fn fcc_send(uint32_t shire, uint32_t thread, uint32_t fcc_reg, uint64_t hart_mask)
    \brief  Send an FCC credit. On ET-SoC1 this targets the shire-wide
    FCC_CREDINC0..3 ESR bank.
*/
static inline __attribute__((always_inline)) void fcc_send(
    uint32_t shire, uint32_t thread, uint32_t fcc_reg, uint64_t hart_mask)
{
    volatile uint64_t *fcc_credinc_addr =
        (uint64_t *)ESR_SHIRE(shire, FCC_CREDINC_0) + ((thread << 1) | fcc_reg);
    *fcc_credinc_addr = hart_mask;
}

/*! \fn flbarrier_set(uint32_t shire, uint32_t barrier_num, uint64_t value)
    \brief  Write a value into an FLB barrier counter. ET-SoC1 barriers are
    shire-wide.
*/
static inline __attribute__((always_inline)) void flbarrier_set(
    uint32_t shire, uint32_t barrier_num, uint64_t value)
{
    volatile uint64_t *flb_addr = (uint64_t *)ESR_SHIRE(shire, FAST_LOCAL_BARRIER0) + barrier_num;
    *flb_addr = value;
}

#ifdef __cplusplus
}
#endif

#endif /* _UTILS_H_ */
