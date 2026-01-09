/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#ifndef BEMU_SPIO_RVTIMER_REGION_H
#define BEMU_SPIO_RVTIMER_REGION_H

#include <cstdint>
#include "emu_defines.h"
#include "devices/rvtimer.h"
#include "memory/memory_region.h"

namespace bemu {

template<unsigned long long Base, unsigned long long N>
struct SpioRVTimerRegion : public MemoryRegion {
    using addr_type     = typename MemoryRegion::addr_type;
    using size_type     = typename MemoryRegion::size_type;
    using value_type    = typename MemoryRegion::value_type;
    using pointer       = typename MemoryRegion::pointer;
    using const_pointer = typename MemoryRegion::const_pointer;

    enum : size_type {
        RVTIMER_REG_MTIME    = 0,
        RVTIMER_REG_MTIMECMP = 8,
    };

    void read(const Agent&, size_type pos, size_type n, pointer result) override {
        if (n == 8) {
            uint64_t *result64 = reinterpret_cast<uint64_t *>(result);
            switch (pos) {
            case RVTIMER_REG_MTIME:
                *result64 = rvtimer.read_mtime();
                break;
            case RVTIMER_REG_MTIMECMP:
                *result64 = rvtimer.read_mtimecmp();
                break;
            default:
                throw memory_error(first() + pos);
            }
            return;
        }

        if (n == 4) {
            uint32_t *result32 = reinterpret_cast<uint32_t *>(result);
            switch (pos) {
            case RVTIMER_REG_MTIME:
                *result32 = static_cast<uint32_t>(rvtimer.read_mtime());
                break;
            case RVTIMER_REG_MTIME + 4:
                *result32 = static_cast<uint32_t>(rvtimer.read_mtime() >> 32);
                break;
            case RVTIMER_REG_MTIMECMP:
                *result32 = static_cast<uint32_t>(rvtimer.read_mtimecmp());
                break;
            case RVTIMER_REG_MTIMECMP + 4:
                *result32 = static_cast<uint32_t>(rvtimer.read_mtimecmp() >> 32);
                break;
            default:
                throw memory_error(first() + pos);
            }
            return;
        }

        throw memory_error(first() + pos);
    }

    void write(const Agent& agent, size_type pos, size_type n, const_pointer source) override {
        if (n == 8) {
            const uint64_t *source64 = reinterpret_cast<const uint64_t *>(source);
            switch (pos) {
            case RVTIMER_REG_MTIME:
                rvtimer.write_mtime(agent, *source64);
                break;
            case RVTIMER_REG_MTIMECMP:
                rvtimer.write_mtimecmp(agent, *source64);
                break;
            default:
                throw memory_error(first() + pos);
            }
            return;
        }

        if (n == 4) {
            const uint32_t *source32 = reinterpret_cast<const uint32_t *>(source);
            switch (pos) {
            case RVTIMER_REG_MTIME: {
                uint64_t cur = rvtimer.read_mtime();
                uint64_t next = (cur & 0xFFFFFFFF00000000ULL) |
                                static_cast<uint64_t>(*source32);
                rvtimer.write_mtime(agent, next);
                break;
            }
            case RVTIMER_REG_MTIME + 4: {
                uint64_t cur = rvtimer.read_mtime();
                uint64_t next = (cur & 0x00000000FFFFFFFFULL) |
                                (static_cast<uint64_t>(*source32) << 32);
                rvtimer.write_mtime(agent, next);
                break;
            }
            case RVTIMER_REG_MTIMECMP: {
                uint64_t cur = rvtimer.read_mtimecmp();
                uint64_t next = (cur & 0xFFFFFFFF00000000ULL) |
                                static_cast<uint64_t>(*source32);
                rvtimer.write_mtimecmp(agent, next);
                break;
            }
            case RVTIMER_REG_MTIMECMP + 4: {
                uint64_t cur = rvtimer.read_mtimecmp();
                uint64_t next = (cur & 0x00000000FFFFFFFFULL) |
                                (static_cast<uint64_t>(*source32) << 32);
                rvtimer.write_mtimecmp(agent, next);
                break;
            }
            default:
                throw memory_error(first() + pos);
            }
            return;
        }

        throw memory_error(first() + pos);
    }

    void init(const Agent&, size_type, size_type, const_pointer) override {
        throw std::runtime_error("bemu::SpioRVTimerRegion::init()");
    }

    addr_type first() const override { return Base; }
    addr_type last() const override { return Base + N - 1; }

    void dump_data(const Agent&, std::ostream&, size_type, size_type) const override { }

    RVTimer<1ull << EMU_IO_SHIRE_SP> rvtimer;
};

} // namespace bemu

#endif // BEMU_SPIO_RVTIMER_REGION_H
