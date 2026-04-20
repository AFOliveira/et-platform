/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent poll-mode UART API.
 *
 * Tests call the normalized helpers below and the platform-specific
 * backend (hal/platform/<soc>/api/uart_platform.h, resolved via -I)
 * picks the actual UART instance and register model.
 */

#ifndef HAL_API_UART_H_
#define HAL_API_UART_H_

#include <stdbool.h>
#include <stdint.h>

#include <api/uart_platform.h>

static inline bool hal_uart_supports_pinmux_gate(void)
{
	return hal_uart_platform_supports_pinmux_gate();
}

static inline void hal_uart_enable_pinmux(void)
{
	hal_uart_platform_enable_pinmux();
}

static inline bool hal_uart_supports_baud_roundtrip(void)
{
	return hal_uart_platform_supports_baud_roundtrip();
}

static inline uint32_t hal_uart_baud_get(void)
{
	return hal_uart_platform_baud_get();
}

static inline void hal_uart_baud_set(uint32_t val)
{
	hal_uart_platform_baud_set(val);
}

static inline bool hal_uart_tx_ready(void)
{
	return hal_uart_platform_tx_ready();
}

static inline bool hal_uart_tx_empty(void)
{
	return hal_uart_platform_tx_empty();
}

static inline bool hal_uart_rx_ready(void)
{
	return hal_uart_platform_rx_ready();
}

static inline uint8_t hal_uart_rx_byte(void)
{
	return hal_uart_platform_rx_byte();
}

static inline void hal_uart_tx_byte(uint8_t c)
{
	hal_uart_platform_tx_byte(c);
}

#endif /* HAL_API_UART_H_ */
