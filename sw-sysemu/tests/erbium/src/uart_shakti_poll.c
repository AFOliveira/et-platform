/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * UART poll-mode RX/TX through the HAL layer. Uses only hal/api/uart.h;
 * the backend picks the actual UART IP:
 *  - Erbium: Shakti UART, with pinmux-gate + baud round-trip sanity
 *    checks driven by hal_uart_supports_pinmux_gate().
 *  - ET-SoC1: DW UART, those capabilities report false and the test
 *    skips them.
 *
 * Expects a single 'A' byte on stdin (see inputs/uart_shakti_poll.in).
 */

#include "test.h"
#include <api/uart.h>
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

		/* Pins are muxed away — this write should be harmless. */
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
