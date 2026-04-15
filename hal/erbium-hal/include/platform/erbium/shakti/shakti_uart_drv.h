/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shakti UART driver — header-only inline implementation.
 * Composes the register-level wrappers in shakti_uart_hal.h into
 * higher-level operations (configure, poll TX/RX, FIFO ops) including
 * the Shakti V3 TX trigger sequence.
 */

#ifndef SHAKTI_UART_DRV_H_
#define SHAKTI_UART_DRV_H_

#include <stddef.h>
#include <stdint.h>

#include <shakti/shakti_uart_hal.h>

struct shakti_uart_cfg {
	uintptr_t base;
	uint16_t baud_divisor;
	uint8_t charsize;
	uint8_t parity;
	uint8_t stopbits;
};

/**
 * Configure UART: set baud, control register (charsize/parity/stopbits),
 * clear delay, IEN, and RX threshold.
 */
static inline int shakti_uart_configure(const struct shakti_uart_cfg *cfg)
{
	if (cfg == NULL || cfg->base == 0U) {
		return -1;
	}

	uintptr_t base = cfg->base;

	/* Set baud rate */
	shakti_uart_baud_set(base, cfg->baud_divisor);

	/* Build control register: charsize, parity, stopbits */
	uint32_t ctrl = UART_CONTROLREG_CHARSIZE_SET(cfg->charsize)
		      | UART_CONTROLREG_PARITY_SET(cfg->parity)
		      | UART_CONTROLREG_STOPBITS_SET(cfg->stopbits);
	shakti_uart_control_set(base, ctrl);

	/* Clear delay, interrupts, and RX threshold */
	shakti_uart_delay_set(base, 0U);
	shakti_uart_int_set(base, 0U);
	shakti_uart_rx_threshold_set(base, 0U);

	return 0;
}

/**
 * Polled TX: wait until TX FIFO is not full, write one byte, trigger.
 * Blocks until the byte can be enqueued.
 */
static inline void shakti_uart_poll_tx(uintptr_t base, uint8_t c,
				       uint16_t baud, uint32_t ien)
{
	while (!shakti_uart_tx_ready(base)) {
		;
	}

	/* V3 TX sequence: write data, then trigger */
	shakti_uart_tx_byte(base, c);
	shakti_uart_tx_trigger(base, baud, ien);
}

/**
 * Polled RX: if a byte is available, read it into *c and return 0.
 * If no data, return -1 without blocking.
 */
static inline int shakti_uart_poll_rx(uintptr_t base, uint8_t *c)
{
	if (!shakti_uart_rx_ready(base)) {
		return -1;
	}

	*c = shakti_uart_rx_byte(base);
	return 0;
}

/**
 * FIFO fill: write up to @len bytes from @data, triggering after each.
 * Stops when TX FIFO is full. Returns number of bytes written.
 */
static inline int shakti_uart_fifo_tx(uintptr_t base, const uint8_t *data,
				      int len, uint16_t baud, uint32_t ien)
{
	int i;

	for (i = 0; i < len; i++) {
		if (!shakti_uart_tx_ready(base)) {
			break;
		}
		shakti_uart_tx_byte(base, data[i]);
		shakti_uart_tx_trigger(base, baud, ien);
	}

	return i;
}

/**
 * FIFO read: read up to @len bytes into @data.
 * Stops when RX FIFO is empty. Returns number of bytes read.
 */
static inline int shakti_uart_fifo_rx(uintptr_t base, uint8_t *data, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if (!shakti_uart_rx_ready(base)) {
			break;
		}
		data[i] = shakti_uart_rx_byte(base);
	}

	return i;
}

#endif /* SHAKTI_UART_DRV_H_ */
