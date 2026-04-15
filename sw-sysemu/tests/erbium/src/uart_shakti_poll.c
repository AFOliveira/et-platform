/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: UART poll-mode RX/TX through the HAL layer.
 *
 * The normalized HAL API keeps the common poll-mode checks portable while
 * allowing the backend to express platform-specific capabilities:
 *
 * - On Erbium, the backend also exposes the legacy pin-mux gate and BAUD
 *   register round-trip used by the original Shakti-focused test.
 * - On ET-SoC1, the backend only exposes the generic poll-mode behaviour
 *   because the UART IP is different.
 *
 * Note: This test expects a single byte ('A') to be provided on stdin by the
 * test runner (see inputs/uart_shakti_poll.in).
 */

#include "test.h"
#include <erbium_hal/uart.h>
#include <stdint.h>

#define UART_RX_TIMEOUT  1000000u

int main(void)
{
	if (hal_uart_supports_pinmux_gate()) {
		if (!hal_uart_tx_empty()) {
			TEST_FAIL;
		}
		if (hal_uart_rx_ready()) {
			TEST_FAIL;
		}
		if (hal_uart_rx_byte() != 0U) {
			TEST_FAIL;
		}

		/* Pins are muxed away, so this write should be harmless. */
		hal_uart_tx_byte('X');

		if (hal_uart_supports_baud_roundtrip()) {
			hal_uart_baud_set(0x1234U);
			if (hal_uart_baud_get() != 0x1234U) {
				TEST_FAIL;
			}
		}

		hal_uart_enable_pinmux();
	}

	if (!hal_uart_tx_ready()) {
		TEST_FAIL;
	}
	if (!hal_uart_tx_empty()) {
		TEST_FAIL;
	}

	for (volatile uint32_t i = 0; i < UART_RX_TIMEOUT; ++i) {
		if (hal_uart_rx_ready()) {
			break;
		}
	}
	if (!hal_uart_rx_ready()) {
		TEST_FAIL;
	}

	if (hal_uart_rx_byte() != 'A') {
		TEST_FAIL;
	}

	if (hal_uart_rx_ready()) {
		TEST_FAIL;
	}

	hal_uart_tx_byte('Z');

	TEST_PASS;
	return 0;
}
