/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: Shakti UART poll-mode RX/TX with pin-mux gating
 *
 * Phase 1 — uart_enable clear (default after reset):
 *   STATUS reports TX_EMPTY but never RX_NOT_EMPTY (pins muxed away).
 *   RCV_REG returns 0. TX write is silently discarded.
 *
 * Phase 2 — uart_enable set:
 *   RX_NOT_EMPTY appears, RX read returns the byte, TX write succeeds.
 *
 * Note: This test expects a single byte ('A') to be provided on stdin by the
 * test runner (see inputs/uart_shakti_poll.in).
 */

#include "test.h"
#include <stdint.h>

#define SYSREG_BASE       0x0002000000ull
#define SYSTEM_CONFIG     (SYSREG_BASE + 0x08)
#define SYSTEM_CONFIG_UART_ENABLE (1u << 6)

#define UART_BASE         0x0002004000ull
#define UART_BAUD         (UART_BASE + 0x00)
#define UART_TX_REG       (UART_BASE + 0x08)
#define UART_RCV_REG      (UART_BASE + 0x10)
#define UART_STATUS       (UART_BASE + 0x18)

#define STATUS_TX_EMPTY     (1u << 0)
#define STATUS_TX_FULL      (1u << 1)
#define STATUS_RX_NOT_EMPTY (1u << 2)

static inline uint32_t uart_read(uint64_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void uart_write(uint64_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

int main(void)
{
    uint32_t status;
    uint32_t cfg;

    /*
     * Phase 1: uart_enable is clear (reset default).
     * Pin I/O should be gated; register access should still work.
     */

    /* STATUS: TX_EMPTY should be set, RX_NOT_EMPTY must not be set */
    status = uart_read(UART_STATUS);
    if ((status & STATUS_TX_EMPTY) == 0)
        TEST_FAIL;
    if (status & STATUS_RX_NOT_EMPTY)
        TEST_FAIL;

    /* RCV_REG returns 0 when pins are muxed away */
    if (uart_read(UART_RCV_REG) != 0)
        TEST_FAIL;

    /* TX write is silently discarded (no crash) */
    uart_write(UART_TX_REG, 'X');

    /* Configuration registers are accessible regardless of pin mux */
    uart_write(UART_BAUD, 0x1234);
    if (uart_read(UART_BAUD) != 0x1234)
        TEST_FAIL;

    /*
     * Phase 2: Set uart_enable. Pin I/O should now work.
     */
    cfg = uart_read(SYSTEM_CONFIG);
    uart_write(SYSTEM_CONFIG, cfg | SYSTEM_CONFIG_UART_ENABLE);

    /* TX should be ready and not full */
    status = uart_read(UART_STATUS);
    if ((status & STATUS_TX_EMPTY) == 0)
        TEST_FAIL;
    if (status & STATUS_TX_FULL)
        TEST_FAIL;

    /* Wait for RX_NOT_EMPTY (byte provided via stdin) */
    for (volatile uint32_t i = 0; i < 1000000; ++i) {
        status = uart_read(UART_STATUS);
        if (status & STATUS_RX_NOT_EMPTY)
            break;
    }
    if ((status & STATUS_RX_NOT_EMPTY) == 0)
        TEST_FAIL;

    /* Read and validate the byte ('A') */
    if ((uart_read(UART_RCV_REG) & 0xffu) != 'A')
        TEST_FAIL;

    /* RX should now be empty */
    status = uart_read(UART_STATUS);
    if (status & STATUS_RX_NOT_EMPTY)
        TEST_FAIL;

    /* Basic TX write should succeed */
    uart_write(UART_TX_REG, 'Z');

    TEST_PASS;
    return 0;
}
