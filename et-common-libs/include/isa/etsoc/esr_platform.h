/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 ESR address encoding.
 *
 *   base           : 0x100000000
 *   bits [31:30]   : PP (privilege)
 *   bits [29:22]   : shire ID (7 bits — up to 34 shires)
 *   bits [21:0]    : sub-region + register offset
 */

#ifndef _ISA_ETSOC_ESR_PLATFORM_H_
#define _ISA_ETSOC_ESR_PLATFORM_H_

#include <stdint.h>

static inline uintptr_t esr_addr(int pp, uint32_t shire,
                                 uint32_t subregion, uint32_t offset)
{
    return (uintptr_t)0x100000000ULL
         | ((uint64_t)(pp & 0x3)     << 30)
         | ((uint64_t)(shire & 0xFF) << 22)
         | ((uint64_t)(subregion & 0x3FFFFFU))
         | ((uint64_t)(offset & 0xFFFFU));
}

#endif /* _ISA_ETSOC_ESR_PLATFORM_H_ */
