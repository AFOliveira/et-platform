/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#ifndef BEMU_RVTIMER_H
#define BEMU_RVTIMER_H

#include <cstdint>
#include <limits>
#include "agent.h"
#include "system.h"

namespace bemu {


template <uint64_t interrupt_shire_mask>
struct RVTimer
{
    RVTimer() {
        reset();
    }

    void reset() {
        mtime = 0;
        prescaler = 0;
        prescaler_threshold = 20; // generate 10MHz from 200MHz
        ref_clock_mux = 0; // unused, only stores value
        mtimecmp = std::numeric_limits<uint64_t>::max();
        interrupt = false;
    }

    bool is_active() const {
        return mtimecmp != std::numeric_limits<uint64_t>::max();
    }

    uint64_t read_mtime() const {
        return mtime;
    }

    void write_mtime(const Agent&, uint64_t val) {
        mtime = val;
    }

    uint64_t read_mtimecmp() const {
        return mtimecmp;
    }

    void write_mtimecmp(const Agent& agent, uint64_t val)
    {
        bool had_interrupt = interrupt;
        mtimecmp = val;
        interrupt = (mtime >= mtimecmp);
        if (had_interrupt && !interrupt) {
            for (unsigned shire = 0; shire < EMU_NUM_SHIRES; ++shire) {
                if ((interrupt_shire_mask >> shire) & 1) {
                    agent.chip->clear_machine_timer_interrupt(shire);
                }
            }
        }
    }

    uint32_t read_time_config() const {
        return (prescaler_threshold & 0x7f) | (ref_clock_mux << 7);
    }

    void write_time_config(const Agent&, uint32_t val) {
        prescaler_threshold = val & 0x7f;
        ref_clock_mux = (val >> 7) & 0x1;
    }

    void clock_tick(const Agent& agent)
    {
        if (++mtime >= mtimecmp) {
            if (!interrupt) {
                for (unsigned shire = 0; shire < EMU_NUM_SHIRES; ++shire) {
                    if ((interrupt_shire_mask >> shire) & 1) {
                        agent.chip->raise_machine_timer_interrupt(shire);
                    }
                }
                interrupt = true;
            }
        }
    }

    void prescaler_tick(const Agent& agent)
    {
        if (++prescaler >= prescaler_threshold) {
            prescaler = 0;
            clock_tick(agent);
        }
    }

private:
    uint64_t mtime;
    uint64_t mtimecmp;
    uint32_t prescaler;
    uint32_t prescaler_threshold;
    uint32_t ref_clock_mux;
    bool interrupt;
};


} // namespace bemu

#endif
