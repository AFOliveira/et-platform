/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium UART backend — Shakti UART at ERBIUM_TOP_UART_REGISTERS_BASE.
 * Pinmux gate lives in system_registers.SystemConfig.UART_ENABLE.
 */

#ifndef _DRIVERS_ERBIUM_UART_PLATFORM_H_
#define _DRIVERS_ERBIUM_UART_PLATFORM_H_

#include <stdbool.h>
#include <stdint.h>

#include "isa/common/mmio.h"
#include "hwinc/system.h"
#include "hwinc/top.h"
#include "hwinc/uart.h"
#include "drivers/erbium/shakti_uart.h"

#define UART_PLATFORM_UART0_BASE   ERBIUM_TOP_UART_REGISTERS_BASE
#define UART_PLATFORM_SYSREG_BASE  ERBIUM_TOP_SYSTEM_REGISTERS_BASE

static inline bool uart_supports_pinmux_gate(void)
{
    return true;
}

static inline void uart_enable_pinmux(void)
{
    uintptr_t const addr = UART_PLATFORM_SYSREG_BASE + SYSTEM_SYSTEMCONFIG_ADDRESS;
    uint32_t cfg = reg_read32(addr);

    cfg = SYSTEM_SYSTEMCONFIG_UART_ENABLE_MODIFY(cfg, 1U);
    reg_write32(addr, cfg);
}

static inline bool uart_supports_baud_roundtrip(void)
{
    return true;
}

static inline uint32_t uart_baud_get(void)
{
    return reg_read32(UART_PLATFORM_UART0_BASE + UART_BAUDREG_ADDRESS);
}

static inline void uart_baud_set(uint32_t val)
{
    shakti_uart_baud_set(UART_PLATFORM_UART0_BASE, (uint16_t)val);
}

static inline bool uart_tx_ready(void)
{
    return shakti_uart_tx_ready(UART_PLATFORM_UART0_BASE);
}

static inline bool uart_tx_empty(void)
{
    return shakti_uart_tx_empty(UART_PLATFORM_UART0_BASE);
}

static inline bool uart_rx_ready(void)
{
    return shakti_uart_rx_ready(UART_PLATFORM_UART0_BASE);
}

static inline uint8_t uart_rx_byte(void)
{
    return shakti_uart_rx_byte(UART_PLATFORM_UART0_BASE);
}

static inline void uart_tx_byte(uint8_t c)
{
    shakti_uart_tx_byte(UART_PLATFORM_UART0_BASE, c);
}

#endif /* _DRIVERS_ERBIUM_UART_PLATFORM_H_ */
