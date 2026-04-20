/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Erbium UART backend — Shakti UART at 0x0002004000, with the
 * pinmux gate in system_registers.SystemConfig.UART_ENABLE.
 */

#ifndef ERBIUM_API_UART_PLATFORM_H_
#define ERBIUM_API_UART_PLATFORM_H_

#include <stdbool.h>
#include <stdint.h>

#include <common/mmio.h>
#include <hwinc/system.h>
#include <hwinc/uart.h>
#include <api/shakti_uart.h>

#define HAL_UART_PLATFORM_UART0_BASE   0x0002004000ull
#define HAL_UART_PLATFORM_SYSREG_BASE  0x0002000000ull

static inline bool hal_uart_platform_supports_pinmux_gate(void)
{
	return true;
}

static inline void hal_uart_platform_enable_pinmux(void)
{
	uintptr_t const addr = HAL_UART_PLATFORM_SYSREG_BASE +
			       SYSTEM_SYSTEMCONFIG_ADDRESS;
	uint32_t cfg = reg_read32(addr);

	cfg = SYSTEM_SYSTEMCONFIG_UART_ENABLE_MODIFY(cfg, 1U);
	reg_write32(addr, cfg);
}

static inline bool hal_uart_platform_supports_baud_roundtrip(void)
{
	return true;
}

static inline uint32_t hal_uart_platform_baud_get(void)
{
	return reg_read32(HAL_UART_PLATFORM_UART0_BASE + UART_BAUDREG_ADDRESS);
}

static inline void hal_uart_platform_baud_set(uint32_t val)
{
	shakti_uart_baud_set(HAL_UART_PLATFORM_UART0_BASE, (uint16_t)val);
}

static inline bool hal_uart_platform_tx_ready(void)
{
	return shakti_uart_tx_ready(HAL_UART_PLATFORM_UART0_BASE);
}

static inline bool hal_uart_platform_tx_empty(void)
{
	return shakti_uart_tx_empty(HAL_UART_PLATFORM_UART0_BASE);
}

static inline bool hal_uart_platform_rx_ready(void)
{
	return shakti_uart_rx_ready(HAL_UART_PLATFORM_UART0_BASE);
}

static inline uint8_t hal_uart_platform_rx_byte(void)
{
	return shakti_uart_rx_byte(HAL_UART_PLATFORM_UART0_BASE);
}

static inline void hal_uart_platform_tx_byte(uint8_t c)
{
	shakti_uart_tx_byte(HAL_UART_PLATFORM_UART0_BASE, c);
}

#endif /* ERBIUM_API_UART_PLATFORM_H_ */
