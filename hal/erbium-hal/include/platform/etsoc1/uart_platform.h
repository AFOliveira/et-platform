/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * ET-SoC1 UART backend.
 *
 * The sys_emu UART model used by Minion tests exposes a simple DW UART view:
 * - RX/TX data at offset 0x0
 * - line status at offset 0x14
 *
 * This backend normalizes that model to the generic poll-mode helpers in
 * <erbium_hal/uart.h>. Baud programming and Erbium-style pin-mux gating are
 * not exposed here, so those helpers are no-ops / unsupported.
 */

#ifndef ETSOC1_PLATFORM_UART_PLATFORM_H_
#define ETSOC1_PLATFORM_UART_PLATFORM_H_

#include <stdbool.h>
#include <stdint.h>

#include <erbium_hal/mmio.h>

#define HAL_UART_PLATFORM_UART0_BASE  0x0012002000ull

#define HAL_UART_PLATFORM_RBR_THR_OFFSET  0x00u
#define HAL_UART_PLATFORM_LSR_OFFSET      0x14u

#define HAL_UART_PLATFORM_LSR_DR    (1u << 0)
#define HAL_UART_PLATFORM_LSR_THRE  (1u << 5)

/*
 * The ET-SoC1 sys_emu UART model marks the RX file descriptor as readable at
 * EOF, so polling DR directly can report stale "data ready" after the last
 * byte has already been consumed. Cache one byte in the HAL backend to
 * normalize poll-mode semantics for tests.
 */
static bool hal_uart_platform_rx_cache_valid;
static uint8_t hal_uart_platform_rx_cache_byte;

static inline bool hal_uart_platform_fill_rx_cache(void)
{
	uint32_t lsr;
	uint8_t byte;

	if (hal_uart_platform_rx_cache_valid) {
		return true;
	}

	lsr = reg_read32(HAL_UART_PLATFORM_UART0_BASE +
			    HAL_UART_PLATFORM_LSR_OFFSET);
	if ((lsr & HAL_UART_PLATFORM_LSR_DR) == 0U) {
		return false;
	}

	byte = (uint8_t)(reg_read32(HAL_UART_PLATFORM_UART0_BASE +
				       HAL_UART_PLATFORM_RBR_THR_OFFSET) &
			 0xffU);
	if (byte == 0U) {
		return false;
	}

	hal_uart_platform_rx_cache_byte = byte;
	hal_uart_platform_rx_cache_valid = true;
	return true;
}

static inline bool hal_uart_platform_supports_pinmux_gate(void)
{
	return false;
}

static inline void hal_uart_platform_enable_pinmux(void)
{
}

static inline bool hal_uart_platform_supports_baud_roundtrip(void)
{
	return false;
}

static inline uint32_t hal_uart_platform_baud_get(void)
{
	return 0U;
}

static inline void hal_uart_platform_baud_set(uint32_t val)
{
	(void)val;
}

static inline bool hal_uart_platform_tx_ready(void)
{
	uint32_t lsr = reg_read32(HAL_UART_PLATFORM_UART0_BASE +
				     HAL_UART_PLATFORM_LSR_OFFSET);

	return (lsr & HAL_UART_PLATFORM_LSR_THRE) == 0U;
}

static inline bool hal_uart_platform_tx_empty(void)
{
	return hal_uart_platform_tx_ready();
}

static inline bool hal_uart_platform_rx_ready(void)
{
	return hal_uart_platform_fill_rx_cache();
}

static inline uint8_t hal_uart_platform_rx_byte(void)
{
	if (!hal_uart_platform_fill_rx_cache()) {
		return 0U;
	}

	hal_uart_platform_rx_cache_valid = false;
	return hal_uart_platform_rx_cache_byte;
}

static inline void hal_uart_platform_tx_byte(uint8_t c)
{
	reg_write32(HAL_UART_PLATFORM_UART0_BASE +
		      HAL_UART_PLATFORM_RBR_THR_OFFSET, (uint32_t)c);
}

#endif /* ETSOC1_PLATFORM_UART_PLATFORM_H_ */
