/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: Shakti UART poll-mode RX/TX basics
 * Expect: RX_NOT_EMPTY set when a byte is present, RX read returns that byte,
 *         TX_EMPTY is set, TX_FULL is clear.
 *
 * Note: This test expects a single byte ('A' by default) to be provided on
 * stdin by the test runner (see Makefile UART_RX_CHAR).
 */

#include "test.h"
#include <stdint.h>

#define UART_BASE         0x0002004000ull
#define UART_TX_REG       (UART_BASE + 0x04)
#define UART_RCV_REG      (UART_BASE + 0x08)
#define UART_STATUS       (UART_BASE + 0x0C)

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

    /* TX should be ready and not full */
    status = uart_read(UART_STATUS);
    if ((status & STATUS_TX_EMPTY) == 0) {
        TEST_FAIL;
    }
    if (status & STATUS_TX_FULL) {
        TEST_FAIL;
    }

    /* Wait for RX_NOT_EMPTY (byte provided via stdin) */
    for (volatile uint32_t i = 0; i < 1000000; ++i) {
        status = uart_read(UART_STATUS);
        if (status & STATUS_RX_NOT_EMPTY) {
            break;
        }
    }
    if ((status & STATUS_RX_NOT_EMPTY) == 0) {
        TEST_FAIL;
    }

    /* Read and validate the byte ('A') */
    if ((uart_read(UART_RCV_REG) & 0xffu) != 'A') {
        TEST_FAIL;
    }

    /* RX should now be empty */
    status = uart_read(UART_STATUS);
    if (status & STATUS_RX_NOT_EMPTY) {
        TEST_FAIL;
    }

    /* Basic TX write should succeed */
    uart_write(UART_TX_REG, 'Z');

    TEST_PASS;
    return 0;
}
