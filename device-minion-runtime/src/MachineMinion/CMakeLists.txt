# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0

set(LINKER_SCRIPT src/sections.ld)
set(LINKER_SCRIPT_DEPENDENCY ${SHARED_INC_DIR}/layout.ld)

add_riscv_executable(MachineMinion
    src/boot.S
    src/trap_handler.S
    src/syscall.c
    src/syscall_ext.c
    src/trap_routine.c
    src/main.c
    src/minion_cfg.c
    src/build_configuration.c
)

#External libs header paths - etsoc_hal.lib, minion-bl.lib
target_include_directories(MachineMinion.elf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src/shared/minion_rt_helpers/include>
)

target_compile_options(MachineMinion.elf
    PRIVATE
        -Wall
        $<$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>:-Werror>
)

# FW_U_ENTRY: M-mode landing pad for `mret` in main.c when launching the
# U-mode compute kernel directly (feat/machineminion-umode-direct).  Default
# matches the Zephyr-U etsoc1_minion_umode SRAM base (0x8006335000) + 0x8e2
# offset where the emulator + gp-sdk crt0 expect to enter.
if(NOT DEFINED FW_U_ENTRY)
    set(FW_U_ENTRY "0x80063358e2ULL")
endif()
target_compile_definitions(MachineMinion.elf PRIVATE FW_U_ENTRY=${FW_U_ENTRY})

target_link_libraries(MachineMinion.elf
    PRIVATE
        et-common-libs::minion-bl
        etsoc_hal::etsoc_hal
        deviceFirmware::minion_rt_helpers_interface
)

target_report_riscv_size(MachineMinion.elf)
target_report_riscv_elf_file_header(MachineMinion.elf)

install(TARGETS MachineMinion.elf
    EXPORT EsperantoDeviceMinionRuntimeTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/esperanto-fw/MachineMinion
    COMPONENT machine-minion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MachineMinion.lst
              ${CMAKE_CURRENT_BINARY_DIR}/MachineMinion.map
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/esperanto-fw/MachineMinion
    COMPONENT machine-minion
)
