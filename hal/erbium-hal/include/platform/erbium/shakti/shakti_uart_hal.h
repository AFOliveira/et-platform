/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shakti UART HAL — stateless inline register access wrappers.
 * OS-agnostic. All functions take uintptr_t base, no OS types.
 *
 * MMIO goes through etsoc_read32/etsoc_write32 (word-sized).
 * Bitfield extraction uses Semifore-style GET/SET/MODIFY macros from
 * the generated hwinc/uart.h header (matches ET-SoC1 HAL conventions).
 */

#ifndef SHAKTI_UART_HAL_H_
#define SHAKTI_UART_HAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <erbium_hal/mmio.h>
#include <hwinc/uart.h>

/* Error flag bits returned by shakti_uart_errors() */
#define SHAKTI_UART_ERR_PARITY   (1U << 0)
#define SHAKTI_UART_ERR_OVERRUN  (1U << 1)
#define SHAKTI_UART_ERR_FRAME    (1U << 2)
#define SHAKTI_UART_ERR_BREAK    (1U << 3)

/* ------------------------------------------------------------------ */
/* Status queries                                                      */
/* ------------------------------------------------------------------ */

static inline uint32_t shakti_uart_status(uintptr_t base)
{
	return etsoc_read32(base + UART_STATUSREG_ADDRESS);
}

static inline bool shakti_uart_tx_ready(uintptr_t base)
{
	return !UART_STATUSREG_TX_FULL_GET(shakti_uart_status(base));
}

static inline bool shakti_uart_tx_empty(uintptr_t base)
{
	return !!UART_STATUSREG_TX_EMPTY_GET(shakti_uart_status(base));
}

static inline bool shakti_uart_rx_ready(uintptr_t base)
{
	return !!UART_STATUSREG_RX_NOTEMPTY_GET(shakti_uart_status(base));
}

/* ------------------------------------------------------------------ */
/* Data transfer                                                       */
/* ------------------------------------------------------------------ */

static inline void shakti_uart_tx_byte(uintptr_t base, uint8_t c)
{
	etsoc_write32(base + UART_TXREG_ADDRESS, (uint32_t)c);
}

static inline uint8_t shakti_uart_rx_byte(uintptr_t base)
{
	return (uint8_t)(etsoc_read32(base + UART_RXREG_ADDRESS) & 0xffU);
}

/* ------------------------------------------------------------------ */
/* V3 TX trigger (HW requirement)                                      */
/* After writing TX_REG, must write BAUD, DELAY, IEN, RX_THRESHOLD    */
/* to start transmission.                                              */
/* ------------------------------------------------------------------ */

static inline void shakti_uart_tx_trigger(uintptr_t base, uint16_t baud,
					  uint32_t ien)
{
	etsoc_write32(base + UART_BAUDREG_ADDRESS, (uint32_t)baud);
	etsoc_write32(base + UART_DELAYREG_ADDRESS, 0U);
	etsoc_write32(base + UART_INTERRUPTEN_ADDRESS, ien);
	etsoc_write32(base + UART_RX_THRESHOLD_ADDRESS, 0U);
}

/* ------------------------------------------------------------------ */
/* Error extraction                                                    */
/* ------------------------------------------------------------------ */

static inline uint32_t shakti_uart_errors(uintptr_t base)
{
	uint32_t s = shakti_uart_status(base);
	uint32_t err = 0U;

	if (UART_STATUSREG_PARITY_ERROR_GET(s)) {
		err |= SHAKTI_UART_ERR_PARITY;
	}
	if (UART_STATUSREG_OVERRUN_ERROR_GET(s)) {
		err |= SHAKTI_UART_ERR_OVERRUN;
	}
	if (UART_STATUSREG_FRAME_ERROR_GET(s)) {
		err |= SHAKTI_UART_ERR_FRAME;
	}
	if (UART_STATUSREG_BREAK_ERROR_GET(s)) {
		err |= SHAKTI_UART_ERR_BREAK;
	}

	return err;
}

/* ------------------------------------------------------------------ */
/* Interrupt enable register                                           */
/* ------------------------------------------------------------------ */

static inline void shakti_uart_int_set(uintptr_t base, uint32_t val)
{
	etsoc_write32(base + UART_INTERRUPTEN_ADDRESS, val);
}

static inline void shakti_uart_int_enable(uintptr_t base, uint32_t *shadow,
					  uint32_t mask)
{
	*shadow |= mask;
	shakti_uart_int_set(base, *shadow);
}

static inline void shakti_uart_int_disable(uintptr_t base, uint32_t *shadow,
					   uint32_t mask)
{
	*shadow &= ~mask;
	shakti_uart_int_set(base, *shadow);
}

static inline bool shakti_uart_irq_pending(uintptr_t base, uint32_t ien_mask)
{
	return !!(shakti_uart_status(base) & ien_mask);
}

/* ------------------------------------------------------------------ */
/* Register writes for configuration                                   */
/* ------------------------------------------------------------------ */

static inline void shakti_uart_baud_set(uintptr_t base, uint16_t divisor)
{
	etsoc_write32(base + UART_BAUDREG_ADDRESS, (uint32_t)divisor);
}

static inline void shakti_uart_control_set(uintptr_t base, uint32_t val)
{
	etsoc_write32(base + UART_CONTROLREG_ADDRESS, val);
}

static inline void shakti_uart_delay_set(uintptr_t base, uint16_t delay)
{
	etsoc_write32(base + UART_DELAYREG_ADDRESS, (uint32_t)delay);
}

static inline void shakti_uart_rx_threshold_set(uintptr_t base, uint8_t level)
{
	etsoc_write32(base + UART_RX_THRESHOLD_ADDRESS, (uint32_t)level);
}

#endif /* SHAKTI_UART_HAL_H_ */
