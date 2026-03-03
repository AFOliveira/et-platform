/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#ifndef BEMU_SHAKTI_UART_H
#define BEMU_SHAKTI_UART_H

#include <array>
#include <cerrno>
#include <cstdint>
#include <system_error>
#include <unistd.h>
#include <sys/select.h>
#include "agent.h"
#include "memory/memory_error.h"
#include "memory/memory_region.h"
#include "system.h"

namespace bemu {

template <unsigned long long Base, size_t N, uint32_t PlicSource>
struct ShaktiUart : public MemoryRegion {
    using addr_type     = typename MemoryRegion::addr_type;
    using size_type     = typename MemoryRegion::size_type;
    using value_type    = typename MemoryRegion::value_type;
    using pointer       = typename MemoryRegion::pointer;
    using const_pointer = typename MemoryRegion::const_pointer;

    // Shakti UART register offsets (64-bit aligned)
    enum : size_type {
        SHAKTI_UART_BAUD         = 0x00,
        SHAKTI_UART_TX_REG       = 0x08,
        SHAKTI_UART_RCV_REG      = 0x10,
        SHAKTI_UART_STATUS       = 0x18,
        SHAKTI_UART_DELAY        = 0x20,
        SHAKTI_UART_CONTROL      = 0x28,
        SHAKTI_UART_IEN          = 0x30,
        SHAKTI_UART_RX_THRESHOLD = 0x40,
    };

    // STATUS register bits
    enum : uint32_t {
        STATUS_TX_EMPTY     = (1u << 0),
        STATUS_TX_FULL      = (1u << 1),
        STATUS_RX_NOT_EMPTY = (1u << 2),
        STATUS_RX_FULL      = (1u << 3),
        STATUS_PARITY_ERROR = (1u << 4),
        STATUS_OVERRUN      = (1u << 5),
        STATUS_FRAME_ERROR  = (1u << 6),
        STATUS_BREAK_ERROR  = (1u << 7),
        STATUS_RXFIFOTHRE   = (1u << 8),
    };

    // IEN register bits
    enum : uint32_t {
        IEN_TX_EMPTY_EN    = (1u << 0),
        IEN_TX_FULL_EN     = (1u << 1),
        IEN_RX_NOT_EMPTY_EN = (1u << 2),
        IEN_RX_FULL_EN     = (1u << 3),
        IEN_PARITY_ERROR_EN = (1u << 4),
        IEN_OVERRUN_EN     = (1u << 5),
        IEN_FRAME_ERROR_EN = (1u << 6),
        IEN_BREAK_ERROR_EN = (1u << 7),
        IEN_RX_THRESHOLD_EN = (1u << 8),
    };

    static constexpr uint32_t IRQ_EVENT_MASK = (1u << 9) - 1u;
    static constexpr size_t RX_FIFO_DEPTH = 16;
    static constexpr uint32_t IRQ_RX_EVENT_MASK =
        IEN_RX_NOT_EMPTY_EN | IEN_RX_FULL_EN | IEN_RX_THRESHOLD_EN;
    static constexpr uint32_t IRQ_RX_POLL_MASK =
        IEN_RX_NOT_EMPTY_EN | IEN_RX_FULL_EN | IEN_RX_THRESHOLD_EN |
        IEN_PARITY_ERROR_EN | IEN_OVERRUN_EN | IEN_FRAME_ERROR_EN |
        IEN_BREAK_ERROR_EN;
    void read(const Agent& agent, size_type pos, size_type n, pointer result) override {
        (void) n;

        switch (pos) {
        case SHAKTI_UART_TX_REG:
            *reinterpret_cast<uint32_t*>(result) = 0;
            break;
        case SHAKTI_UART_RCV_REG: {
            uint8_t data = 0;
            if (agent.chip->is_uart_enabled()) {
                poll_rx_fifo();
                (void)rx_fifo_pop(data);
            }
            *reinterpret_cast<uint32_t*>(result) = data;
            sync_interrupt_line(agent, false);
            break;
        }
        case SHAKTI_UART_STATUS: {
            if (agent.chip->is_uart_enabled()) {
                *reinterpret_cast<uint32_t*>(result) = status_value(true);
            } else {
                *reinterpret_cast<uint32_t*>(result) = STATUS_TX_EMPTY;
            }
            // Error status bits are latched and clear-on-read.
            error_overrun = false;
            sync_interrupt_line(agent, false);
            break;
        }
        case SHAKTI_UART_BAUD:
            *reinterpret_cast<uint32_t*>(result) = reg_baud;
            break;
        case SHAKTI_UART_DELAY:
            *reinterpret_cast<uint32_t*>(result) = reg_delay;
            break;
        case SHAKTI_UART_CONTROL:
            *reinterpret_cast<uint32_t*>(result) = reg_control;
            break;
        case SHAKTI_UART_IEN:
            *reinterpret_cast<uint32_t*>(result) = reg_ien;
            break;
        case SHAKTI_UART_RX_THRESHOLD:
            *reinterpret_cast<uint32_t*>(result) = reg_rx_threshold;
            break;
        default:
            *reinterpret_cast<uint32_t*>(result) = 0;
            break;
        }
    }

    void write(const Agent& agent, size_type pos, size_type n, const_pointer source) override {
        (void) n;

        uint32_t value = *reinterpret_cast<const uint32_t*>(source);

        switch (pos) {
        case SHAKTI_UART_TX_REG:
            if (agent.chip->is_uart_enabled() && (tx_fd != -1) && (::write(tx_fd, source, 1) < 0)) {
                auto error = std::error_code(errno, std::system_category());
                throw std::system_error(error, "bemu::ShaktiUart::write()");
            }
            break;
        case SHAKTI_UART_BAUD:
            reg_baud = value & 0xFFFFu;
            break;
        case SHAKTI_UART_DELAY:
            reg_delay = value & 0xFFFFu;
            break;
        case SHAKTI_UART_CONTROL:
            reg_control = value & 0xFFFFu;
            break;
        case SHAKTI_UART_IEN:
            reg_ien = value & IRQ_EVENT_MASK;
            sync_interrupt_line(agent, true);
            break;
        case SHAKTI_UART_RX_THRESHOLD:
            reg_rx_threshold = value & 0xFFu;
            sync_interrupt_line(agent, true);
            break;
        case SHAKTI_UART_STATUS:
            // Read-only, ignore writes
            break;
        default:
            break;
        }
    }

    void init(const Agent&, size_type, size_type, const_pointer) override {
        throw std::runtime_error("bemu::ShaktiUart::init()");
    }

    addr_type first() const override { return Base; }
    addr_type last() const override { return Base + N - 1; }

    void dump_data(const Agent&, std::ostream&, size_type, size_type) const override { }

    void clock_tick(const Agent& agent) {
        if ((reg_ien & IRQ_RX_POLL_MASK) == 0) {
            return;
        }
        sync_interrupt_line(agent, true);
    }

    bool interrupt_monitoring_active() const {
        return (rx_fd != -1) && ((reg_ien & IRQ_RX_POLL_MASK) != 0);
    }

    int tx_fd = -1;
    int rx_fd = -1;

private:
    uint32_t reg_baud = 0;
    uint32_t reg_delay = 0;
    uint32_t reg_control = 0;
    uint32_t reg_ien = 0;
    uint32_t reg_rx_threshold = 0;
    std::array<uint8_t, RX_FIFO_DEPTH> rx_fifo{};
    size_t   rx_fifo_head = 0;
    size_t   rx_fifo_tail = 0;
    size_t   rx_fifo_count = 0;
    bool     error_overrun = false;
    bool     interrupt_asserted = false;

    uint32_t status_value(bool poll_rx) {
        if (poll_rx) {
            poll_rx_fifo();
        }

        uint32_t status = STATUS_TX_EMPTY;
        if (rx_fifo_count > 0) {
            status |= STATUS_RX_NOT_EMPTY;
        }
        if (rx_fifo_count >= RX_FIFO_DEPTH) {
            status |= STATUS_RX_FULL;
        }
        if (error_overrun) {
            status |= STATUS_OVERRUN;
        }
        if (rx_threshold_reached()) {
            status |= STATUS_RXFIFOTHRE;
        }
        return status;
    }

    void sync_interrupt_line(const Agent& agent, bool poll_rx) {
        if (agent.chip == nullptr) {
            return;
        }

        const uint32_t status = status_value(poll_rx);
        const bool should_raise = ((status & reg_ien & IRQ_EVENT_MASK) != 0);
        if (should_raise == interrupt_asserted) {
            return;
        }

        interrupt_asserted = should_raise;
        if (should_raise) {
            agent.chip->plic_interrupt_pending_set(PlicSource);
        } else {
            agent.chip->plic_interrupt_pending_clear(PlicSource);
        }
    }

    bool rx_threshold_reached() const {
        if (reg_rx_threshold == 0) {
            return rx_fifo_count > 0;
        }
        return rx_fifo_count > reg_rx_threshold;
    }

    bool rx_fifo_push(uint8_t value) {
        if (rx_fifo_count >= RX_FIFO_DEPTH) {
            error_overrun = true;
            return false;
        }
        rx_fifo[rx_fifo_tail] = value;
        rx_fifo_tail = (rx_fifo_tail + 1) % RX_FIFO_DEPTH;
        ++rx_fifo_count;
        return true;
    }

    bool rx_fifo_pop(uint8_t& value) {
        if (rx_fifo_count == 0) {
            return false;
        }
        value = rx_fifo[rx_fifo_head];
        rx_fifo_head = (rx_fifo_head + 1) % RX_FIFO_DEPTH;
        --rx_fifo_count;
        return true;
    }

    void poll_rx_fifo() {
        while (true) {
            uint8_t value = 0;
            const int result = read_rx_byte_nonblocking(value);
            if (result != 1) {
                break;
            }
            (void)rx_fifo_push(value);
        }
    }

    // select() with timeout=0 checks if read() would block. However, it
    // returns "readable" both for actual data and for EOF — so select()
    // alone cannot tell them apart. We follow up with read() to distinguish:
    //   r == 1 : real byte  — buffer it for the guest
    //   r == 0 : EOF        — set rx_fd = -1, stop polling
    //   r <  0 : error      — no data
    int read_rx_byte_nonblocking(uint8_t& value) {
        if (rx_fd == -1) return 0;

        fd_set rfds;
        struct timeval tv = {0, 0};
        FD_ZERO(&rfds);
        FD_SET(rx_fd, &rfds);
        if (select(rx_fd + 1, &rfds, nullptr, nullptr, &tv) <= 0)
            return 0;

        ssize_t r = ::read(rx_fd, &value, 1);
        if (r == 1) { return 1; }
        if (r == 0) { rx_fd = -1; return 0; }
        return -1;
    }
};


} // namespace bemu

#endif // BEMU_SHAKTI_UART_H
