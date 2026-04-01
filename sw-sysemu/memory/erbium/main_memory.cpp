/*-------------------------------------------------------------------------
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#include "memory/erbium/main_memory.h"
#include "emu_defines.h"
#include "devices/plic_er.h"
#include "devices/shakti_uart.h"
#include "devices/sysregs_er.h"
#include "system.h"
#include "memory/sysreg_region.h"
#include "memory/dense_region.h"

namespace bemu {

template<unsigned long long Base, unsigned long long N>
struct RegisterStubRegion : public MemoryRegion {
    using addr_type     = typename MemoryRegion::addr_type;
    using size_type     = typename MemoryRegion::size_type;
    using value_type    = typename MemoryRegion::value_type;
    using pointer       = typename MemoryRegion::pointer;
    using const_pointer = typename MemoryRegion::const_pointer;
    using storage_type  = lazy_array<value_type, N>;

    static_assert(N > 0, "bemu::RegisterStubRegion size must be non-zero");

    void read(const Agent& agent, size_type pos, size_type n, pointer result) override {
        if (storage.empty()) {
            default_value(result, n, agent.chip->memory_reset_value, pos);
        } else {
            std::copy_n(storage.cbegin() + pos, n, result);
        }
    }

    void write(const Agent& agent, size_type pos, size_type n, const_pointer source) override {
        init(agent, pos, n, source);
    }

    void init(const Agent& agent, size_type pos, size_type n, const_pointer source) override {
        if (storage.empty()) {
            storage.allocate();
            storage.fill_pattern(agent.chip->memory_reset_value, MEM_RESET_PATTERN_SIZE);
        }
        std::copy_n(source, n, storage.begin() + pos);
    }

    addr_type first() const override { return Base; }
    addr_type last() const override { return Base + N - 1; }

    void dump_data(const Agent& agent, std::ostream& os, size_type pos, size_type n) const override {
        bemu::dump_data(os, storage, pos, n, agent.chip->memory_reset_value[0]);
    }

    storage_type storage;
};

void MainMemory::reset()
{
    regions[erbreg_idx].reset(new SysregsEr<region_bases[erbreg_idx]>());
    regions[mram_bridge_idx].reset(new DenseRegion<region_bases[mram_bridge_idx], region_sizes[mram_bridge_idx]>());
    regions[i2c_idx].reset(new RegisterStubRegion<region_bases[i2c_idx], region_sizes[i2c_idx]>());
    regions[qspi_idx].reset(new RegisterStubRegion<region_bases[qspi_idx], region_sizes[qspi_idx]>());
    regions[uart_idx].reset(new ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>());
    regions[bootrom_idx].reset(new DenseRegion<region_bases[bootrom_idx], region_sizes[bootrom_idx], false>());
    regions[sram_idx].reset(new DenseRegion<region_bases[sram_idx], region_sizes[sram_idx]>());
    regions[xspi_idx].reset(new RegisterStubRegion<region_bases[xspi_idx], region_sizes[xspi_idx]>());
    regions[dram_idx].reset(new DenseRegion<region_bases[dram_idx], region_sizes[dram_idx]>());
    regions[otp_idx].reset(new DenseRegion<region_bases[otp_idx], region_sizes[otp_idx], false>());
    regions[sysreg_idx].reset(new SysregRegion<region_bases[sysreg_idx], region_sizes[sysreg_idx]>());
    regions[plic_idx].reset(new ER_PLIC<region_bases[plic_idx], region_sizes[plic_idx]>());
    regions[nic_idx].reset(new DenseRegion<region_bases[nic_idx], region_sizes[nic_idx]>());
}

void MainMemory::wdt_clock_tick(const Agent& agent, uint64_t cycle)
{
    auto ptr = dynamic_cast<SysregsEr<region_bases[erbreg_idx]>*>(regions[erbreg_idx].get());
    ptr->wdt_clock_tick(agent, cycle);
}

inline auto& MainMemory::rvtimer() const {
    auto ptr = dynamic_cast<SysregRegion<region_bases[sysreg_idx], region_sizes[sysreg_idx]>*>(regions[sysreg_idx].get());
    return ptr->rvtimer;
}

bool MainMemory::rvtimer_is_active() const {
    return rvtimer().is_active();
}

uint64_t MainMemory::rvtimer_read_mtime() const {
    return rvtimer().read_mtime();
}

uint64_t MainMemory::rvtimer_read_mtimecmp() const {
    return rvtimer().read_mtimecmp();
}

uint64_t MainMemory::rvtimer_read_time_config() const {
    return rvtimer().read_time_config();
}

void MainMemory::rvtimer_clock_tick(const Agent& agent, uint64_t cycle) {
    // cycle at 200MHz
    if ((cycle % 5) == 0) {
        rvtimer().prescaler_tick(agent);
    }
}

void MainMemory::rvtimer_write_mtime(const Agent& agent, uint64_t value) {
    rvtimer().write_mtime(agent, value);
}

void MainMemory::rvtimer_write_mtimecmp(const Agent& agent, uint64_t value) {
    rvtimer().write_mtimecmp(agent, value);
}

void MainMemory::rvtimer_write_time_config(const Agent& agent, uint64_t value) {
    rvtimer().write_time_config(agent, value);
}

void MainMemory::rvtimer_reset() {
    rvtimer().reset();
}

void MainMemory::uart_set_tx_fd(int fd) {
    auto ptr = dynamic_cast<ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>*>(regions[uart_idx].get());
    ptr->tx_fd = fd;
}

void MainMemory::uart_set_rx_fd(int fd) {
    auto ptr = dynamic_cast<ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>*>(regions[uart_idx].get());
    ptr->rx_fd = fd;
}

int MainMemory::uart_get_tx_fd() const {
    auto ptr = dynamic_cast<ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>*>(regions[uart_idx].get());
    return ptr->tx_fd;
}

int MainMemory::uart_get_rx_fd() const {
    auto ptr = dynamic_cast<ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>*>(regions[uart_idx].get());
    return ptr->rx_fd;
}

bool MainMemory::is_uart_enabled() const {
    auto ptr = dynamic_cast<SysregsEr<region_bases[erbreg_idx]>*>(regions[erbreg_idx].get());
    return ptr->is_uart_enabled();
}

void MainMemory::uart_clock_tick(const Agent& agent, uint64_t cycle) {
    auto ptr = dynamic_cast<ShaktiUart<region_bases[uart_idx], region_sizes[uart_idx], ER_PLIC_UART0_INTR_ID>*>(regions[uart_idx].get());
    ptr->clock_tick(agent, cycle);
}

void MainMemory::plic_interrupt_pending_set(const Agent& agent, uint32_t source)
{
    auto ptr = dynamic_cast<ER_PLIC<region_bases[plic_idx], region_sizes[plic_idx]>*>(regions[plic_idx].get());
    ptr->interrupt_pending_set(agent, source);
}

void MainMemory::plic_interrupt_pending_clear(const Agent& agent, uint32_t source)
{
    auto ptr = dynamic_cast<ER_PLIC<region_bases[plic_idx], region_sizes[plic_idx]>*>(regions[plic_idx].get());
    ptr->interrupt_pending_clear(agent, source);
}

} // namespace bemu
