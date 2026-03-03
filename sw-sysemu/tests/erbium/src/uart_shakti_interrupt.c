/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

/*
 * Test: Shakti UART RX interrupt through Erbium PLIC
 * Expected: PASS (machine external interrupt trap)
 */

#include "test.h"
#include "trap.h"
#include <stdint.h>

#define UART_BASE                 0x0002004000ull
#define UART_IEN                  (UART_BASE + 0x30)
#define UART_RX_THRESHOLD         (UART_BASE + 0x40)

#define UART_IEN_RX_NOT_EMPTY_EN  (1u << 2)

#define PLIC_BASE                 0xA0000000UL
#define PLIC_PRIORITY_BASE        0x000000UL
#define PLIC_ENABLE_BASE          0x002000UL
#define PLIC_THRESHOLD_BASE       0x200000UL

#define PLIC_PRIORITY(src)        (*(volatile uint32_t *)(PLIC_BASE + PLIC_PRIORITY_BASE + (src) * 4))
#define PLIC_ENABLE(ctx, word)    (*(volatile uint32_t *)(PLIC_BASE + PLIC_ENABLE_BASE + (ctx) * 0x80 + (word) * 4))
#define PLIC_THRESHOLD(ctx)       (*(volatile uint32_t *)(PLIC_BASE + PLIC_THRESHOLD_BASE + (ctx) * 0x1000))

#define ER_PLIC_UART0_SOURCE_ID   3u
#define PLIC_MACHINE_CONTEXT      0u

#define MIE_MEIE                  (1UL << 11)
#define MSTATUS_MIE               (1UL << 3)

static inline void uart_write(uint64_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

int main(void)
{
    /* Route UART source to machine context 0 with non-zero priority. */
    PLIC_PRIORITY(ER_PLIC_UART0_SOURCE_ID) = 1;
    PLIC_ENABLE(PLIC_MACHINE_CONTEXT, 0) = (1u << ER_PLIC_UART0_SOURCE_ID);
    PLIC_THRESHOLD(PLIC_MACHINE_CONTEXT) = 0;

    /* Enable UART RX-not-empty interrupt generation. */
    uart_write(UART_RX_THRESHOLD, 0);
    uart_write(UART_IEN, UART_IEN_RX_NOT_EMPTY_EN);

    expect_exception(CAUSE_MACHINE_EXTERNAL_INTERRUPT);

    asm volatile("csrs mie, %0" :: "r"(MIE_MEIE));
    asm volatile("csrs mstatus, %0" :: "r"(MSTATUS_MIE));

    /* Wait for stdin byte to be sampled by UART and forwarded through PLIC. */
    for (volatile uint32_t i = 0; i < 1000000; ++i) {
        asm volatile("nop");
    }

    TEST_FAIL;
    return 0;
}
