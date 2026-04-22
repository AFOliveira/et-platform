/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 IPI backend — trigger / trigger-clear via ESR shire_other.
 */

#ifndef _DRIVERS_ETSOC_IPI_PLATFORM_H_
#define _DRIVERS_ETSOC_IPI_PLATFORM_H_

#include <stdint.h>

#include "isa/common/esr.h"
#include "hwinc/etsoc_shire_other_esr.h"

static inline uint64_t ipi_read_trigger(void)
{
    return esr_read_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                        ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_ADDRESS);
}

static inline void ipi_write_trigger(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_ADDRESS, val);
}

static inline void ipi_write_trigger_clear(uint64_t val)
{
    esr_write_u64(ESR_PP_MACHINE, 0, ESR_SR_CPU,
                  ETSOC_SHIRE_OTHER_ESR_IPI_TRIGGER_CLEAR_ADDRESS, val);
}

#endif /* _DRIVERS_ETSOC_IPI_PLATFORM_H_ */
