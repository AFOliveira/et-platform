/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#ifndef BEMU_SHAKTI_UART_H
#define BEMU_SHAKTI_UART_H

#include <cassert>
#include <cerrno>
#include <cinttypes>
#include <cstdint>
#include <system_error>
#include <unistd.h>
#include "memory/memory_error.h"
#include "memory/memory_region.h"

namespace bemu {

template <unsigned long long Base, size_t N>
struct ShaktiUart : public MemoryRegion {
    using addr_type     = typename MemoryRegion::addr_type;
    using size_type     = typename MemoryRegion::size_type;
    using value_type    = typename MemoryRegion::value_type;
    using pointer       = typename MemoryRegion::pointer;
    using const_pointer = typename MemoryRegion::const_pointer;

    // Shakti UART register offsets
    enum : size_type {
        SHAKTI_UART_BAUD         = 0x00,
        SHAKTI_UART_TX_REG       = 0x04,
        SHAKTI_UART_RCV_REG      = 0x08,
        SHAKTI_UART_STATUS       = 0x0C,
        SHAKTI_UART_DELAY        = 0x10,
        SHAKTI_UART_CONTROL      = 0x14,
        SHAKTI_UART_IEN          = 0x18,
        SHAKTI_UART_RX_THRESHOLD = 0x20,
    };

    // STATUS register bits
    enum : uint32_t {
        STATUS_TX_EMPTY     = (1u << 0),
        STATUS_TX_FULL      = (1u << 1),
        STATUS_RX_NOT_EMPTY = (1u << 2),
        STATUS_RX_FULL      = (1u << 3),
    };

    void read(const Agent&, size_type pos, size_type n, pointer result) override {
        (void) n;
        assert(n == 4);

        switch (pos) {
        case SHAKTI_UART_TX_REG:
            *reinterpret_cast<uint32_t*>(result) = 0;
            break;
        case SHAKTI_UART_RCV_REG: {
            char data = 0;
            if (rx_fd != -1 && fd_read_data_available(rx_fd)) {
                auto res = ::read(rx_fd, &data, 1);
                (void)res;
            }
            *reinterpret_cast<uint32_t*>(result) = static_cast<uint8_t>(data);
            break;
        }
        case SHAKTI_UART_STATUS: {
            uint32_t status = STATUS_TX_EMPTY; // TX always ready
            if (rx_fd != -1 && fd_read_data_available(rx_fd)) {
                status |= STATUS_RX_NOT_EMPTY;
            }
            *reinterpret_cast<uint32_t*>(result) = status;
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

    void write(const Agent&, size_type pos, size_type n, const_pointer source) override {
        (void) n;
        assert(n == 4);

        uint32_t value = *reinterpret_cast<const uint32_t*>(source);

        switch (pos) {
        case SHAKTI_UART_TX_REG:
            if ((tx_fd != -1) && (::write(tx_fd, source, 1) < 0)) {
                auto error = std::error_code(errno, std::system_category());
                throw std::system_error(error, "bemu::ShaktiUart::write()");
            }
            break;
        case SHAKTI_UART_BAUD:
            reg_baud = value;
            break;
        case SHAKTI_UART_DELAY:
            reg_delay = value;
            break;
        case SHAKTI_UART_CONTROL:
            reg_control = value;
            break;
        case SHAKTI_UART_IEN:
            reg_ien = value;
            break;
        case SHAKTI_UART_RX_THRESHOLD:
            reg_rx_threshold = value;
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

    int tx_fd = -1;
    int rx_fd = -1;

private:
    uint32_t reg_baud = 0;
    uint32_t reg_delay = 0;
    uint32_t reg_control = 0;
    uint32_t reg_ien = 0;
    uint32_t reg_rx_threshold = 0;

    static bool fd_read_data_available(int fd) {
        fd_set rfds;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        if (select(fd + 1, &rfds, nullptr, nullptr, &tv) < 0) {
            return false;
        }
        return FD_ISSET(fd, &rfds);
    }
};


} // namespace bemu

#endif // BEMU_SHAKTI_UART_H
