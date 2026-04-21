/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Validation-CSR diagnostics used by tests to drive internal state the
 * architectural ISA cannot reach (forcing IRQs pending, etc.). Not part
 * of the HAL: this is emulator/validation scaffolding, not an SoC
 * abstraction.
 */

#ifndef ERBIUM_TESTS_DIAG_H
#define ERBIUM_TESTS_DIAG_H

#include <stdint.h>
#include "csr.h"

#define DIAG_OP_IRQ_INJ         0x5ULL
#define DIAG_IRQ_INJ_PLIC       0x3ULL

#define DIAG_OP_LSB             56
#define DIAG_IRQ_INJ_RAISE_LSB  55
#define DIAG_IRQ_INJ_TARGET_LSB 53

static inline void diag_plic_set_pending(uint32_t source_id, uint32_t raise)
{
    uint64_t cmd = (DIAG_OP_IRQ_INJ           << DIAG_OP_LSB)
                 | ((uint64_t)(raise & 1)     << DIAG_IRQ_INJ_RAISE_LSB)
                 | (DIAG_IRQ_INJ_PLIC         << DIAG_IRQ_INJ_TARGET_LSB)
                 | source_id;
    csr_write(validation1, cmd);
}

#endif /* ERBIUM_TESTS_DIAG_H */
