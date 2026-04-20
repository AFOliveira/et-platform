/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*
* ET-SoC1 linker script. Minions reset to 0x8000001000 (DRAM + 0x1000),
* per sw-sysemu/esrs_et.cpp ESR_NEIGH_MINION_BOOT_RESET_VAL. .text.init
* lands there; everything else follows in DRAM.
*-------------------------------------------------------------------------*/

OUTPUT_ARCH("riscv")
ENTRY(_boot)

DRAM_BASE = 0x8000001000;
DRAM_SIZE = 1024M;

/* Stack configuration (matches Erbium's) */
MAX_THREADS     = 16;
STACK_SIZE      = 64;
STACK_SIZE_LOG2 = 6;

MEMORY {
    DRAM (rwx) : org = DRAM_BASE, len = DRAM_SIZE
}

SECTIONS {
    .text : {
        . = ALIGN(8);
        PROVIDE(__code_start = .);
        *(.text.init)
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(8);
        PROVIDE(__code_end = .);
    } > DRAM

    .data : {
        . = ALIGN(8);
        PROVIDE(__data_start = .);
        *(.data .data.*)
    } > DRAM

    .sdata : {
        __global_pointer$ = . + 0x800;
        . = ALIGN(8);
        *(.sdata .sdata.*)
        PROVIDE(__data_end = .);
    } > DRAM

    .bss (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE(__bss_start = .);
        *(.bss .bss.*)
    } > DRAM

    .sbss (NOLOAD) : {
        *(.sbss .sbss.*)
        . = ALIGN(8);
        PROVIDE(__bss_end = .);
    } > DRAM

    .stack (NOLOAD) : {
        . = ALIGN(16);
        PROVIDE(__stack_start = .);
        . = . + (MAX_THREADS * STACK_SIZE);
        PROVIDE(__stack_base = .);
    } > DRAM
}
