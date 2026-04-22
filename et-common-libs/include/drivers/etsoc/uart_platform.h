/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 UART backend.
 *
 * The sys_emu UART model used by Minion tests exposes a simple DW UART
 * view — RX/TX data at offset 0x0, line status at offset 0x14. This
 * backend normalizes that to the generic poll-mode helpers in
 * drivers/common/uart.h. Baud programming and pin-mux gating are not
 * exposed by the emulator model; those helpers are no-ops / unsupported.
 *
 * The stateful rx_cache workaround exists because sys_emu reports LSR.DR=1
 * at file-descriptor EOF — polling LSR.DR directly can report a stale
 * "data ready" after the last byte. Caching one byte normalizes poll-mode
 * semantics for tests.
 */

#ifndef _DRIVERS_ETSOC_UART_PLATFORM_H_
#define _DRIVERS_ETSOC_UART_PLATFORM_H_

#include <stdbool.h>
#include <stdint.h>

#include "isa/common/mmio.h"
#include "hwinc/hal_device.h"

#define UART_PLATFORM_UART0_BASE  ((uintptr_t)R_PU_UART_BASEADDR)

#define UART_PLATFORM_RBR_THR_OFFSET  0x00u
#define UART_PLATFORM_LSR_OFFSET      0x14u

#define UART_PLATFORM_LSR_DR    (1u << 0)
#define UART_PLATFORM_LSR_THRE  (1u << 5)

static bool uart_platform_rx_cache_valid;
static uint8_t uart_platform_rx_cache_byte;

static inline bool uart_platform_fill_rx_cache(void)
{
    uint32_t lsr;
    uint8_t byte;

    if (uart_platform_rx_cache_valid) {
        return true;
    }

    lsr = reg_read32(UART_PLATFORM_UART0_BASE + UART_PLATFORM_LSR_OFFSET);
    if ((lsr & UART_PLATFORM_LSR_DR) == 0U) {
        return false;
    }

    byte = (uint8_t)(reg_read32(UART_PLATFORM_UART0_BASE +
                                UART_PLATFORM_RBR_THR_OFFSET) & 0xffU);
    if (byte == 0U) {
        return false;
    }

    uart_platform_rx_cache_byte = byte;
    uart_platform_rx_cache_valid = true;
    return true;
}

static inline bool uart_supports_pinmux_gate(void)
{
    return false;
}

static inline void uart_enable_pinmux(void)
{
}

static inline bool uart_supports_baud_roundtrip(void)
{
    return false;
}

static inline uint32_t uart_baud_get(void)
{
    return 0U;
}

static inline void uart_baud_set(uint32_t val)
{
    (void)val;
}

static inline bool uart_tx_ready(void)
{
    uint32_t lsr = reg_read32(UART_PLATFORM_UART0_BASE + UART_PLATFORM_LSR_OFFSET);

    return (lsr & UART_PLATFORM_LSR_THRE) == 0U;
}

static inline bool uart_tx_empty(void)
{
    return uart_tx_ready();
}

static inline bool uart_rx_ready(void)
{
    return uart_platform_fill_rx_cache();
}

static inline uint8_t uart_rx_byte(void)
{
    if (!uart_platform_fill_rx_cache()) {
        return 0U;
    }

    uart_platform_rx_cache_valid = false;
    return uart_platform_rx_cache_byte;
}

static inline void uart_tx_byte(uint8_t c)
{
    reg_write32(UART_PLATFORM_UART0_BASE + UART_PLATFORM_RBR_THR_OFFSET, (uint32_t)c);
}

#endif /* _DRIVERS_ETSOC_UART_PLATFORM_H_ */
