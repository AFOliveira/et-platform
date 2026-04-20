/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*
* ET-SoC1 trap handler — same logic as common/trap.S but lives in
* regular .text (no dedicated traprom region).
*-------------------------------------------------------------------------*/

  .section .text, "ax", @progbits
  .global default_trap_handler
  .align 2

default_trap_handler:
  csrr t0, mcause
  csrr t1, mscratch
  beqz t1, trap_unexpected
  bne  t0, t1, trap_unexpected

trap_expected:
  li   a7, 0x1FEED000         /* PASS */
  j    trap_done

trap_unexpected:
  li   a7, 0x50BAD000         /* FAIL */

trap_done:
  csrw validation0, a7
  wfi
  j    trap_done
