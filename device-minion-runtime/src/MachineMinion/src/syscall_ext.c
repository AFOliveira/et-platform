/*-------------------------------------------------------------------------
* Copyright (c) 2026 AIFoundry
* SPDX-License-Identifier: Apache-2.0
*
* MachineMinion OS-kernel syscall extensions + U-mode IRQ upcall.
*
* These extend the cm-umode ABI for Zephyr-U.  IDs 50..57 (see
* et-common-libs/include/isa/common/syscall.h, SYSCALL_OSKERN_*) provide
* a minimal privileged surface for a U-mode kernel: PLIC enable/disable,
* global MIE mask/unmask, upcall registration, upcall completion, timer
* arming, console write.
*
* The upcall mechanism is the hard bit.  Flow:
*
*   [U-mode Zephyr runs, mstatus.MIE set to allow M-mode IRQs to trap]
*        |
*        v
*   M-mode external interrupt arrives
*        |
*        v
*   trap_handler.S fast-path miss -> slow_path -> mcause bit 63 set and
*        cause-low == 11 -> jal mext_interrupt_handler(regs)
*        |
*        v
*   mext_interrupt_handler():
*     - claim PLIC (get irq_num)
*     - save regs, mepc, mstatus into g_u_irq_save
*     - rewrite trap frame: regs[10] = irq_num (a0)
*     - rewrite mepc to g_u_irq_handler (registered U entry point)
*        |
*        v
*   trap_handler.S resumes, restores the (modified) frame and mrets
*        |
*        v
*   U-mode IRQ handler runs with a0 = irq_num
*        |
*        v
*   U-mode IRQ handler done -> syscall(SYSCALL_OSKERN_IRQ_COMPLETE, irq_num)
*        |
*        v
*   ecall from U -> trap_handler slow_path -> syscall_handler_umode()
*     (new regs pointer arg) -> oskern_irq_complete(regs, irq_num):
*        - PLIC complete irq_num
*        - copy g_u_irq_save back into trap frame
*     - trap_handler advances mepc +4 from the COMPLETE frame (BUT we
*       overwrote mepc via mstatus/mepc CSRs to the saved values, so
*       the existing slow_path post-syscall mepc+=4 is wrong for this
*       path).  We set a flag to skip the +4 in the assembly.
*        |
*        v
*   trap_handler.S restores (restored) frame and mrets back to where
*   U-mode originally was.
*
* Limitations (MVP):
*   - Single-level upcall: if a second M-mode IRQ fires before IRQ_COMPLETE
*     the saved context is preserved only for the first.  For the demo we
*     mitigate by driving only the timer IRQ and claim-completing it.
*   - Nested IRQs not supported.
*------------------------------------------------------------------------- */

#include <stdint.h>
#include <stdbool.h>

#include <isa/common/syscall.h>
#include <isa/common/riscv_encoding.h>

#include "syscall_internal.h"

/* hwinc: PU PLIC base, UART base, RV timer base */
#include <hwinc/hal_device.h>
#include <hwinc/pu_rvtim.h>

/* Forward declarations (visible to callers in trap_handler.S / syscall.c) */
int64_t oskern_syscall_dispatch(uint64_t number, uint64_t a1, uint64_t a2,
                                 uint64_t a3, uint64_t *regs);
void mext_interrupt_handler(uint64_t *regs);

/* ------------------------------------------------------------------ */
/* MMIO helpers                                                       */
/* ------------------------------------------------------------------ */

static inline uint32_t mm_mmio_read32(uintptr_t addr)
{
    return *(volatile uint32_t *)addr;
}

static inline void mm_mmio_write32(uintptr_t addr, uint32_t val)
{
    *(volatile uint32_t *)addr = val;
}

/* ------------------------------------------------------------------ */
/* PLIC register layout (standard RISC-V PLIC, PU variant)            */
/* ------------------------------------------------------------------ */

#define PU_PLIC_BASE          ((uintptr_t)R_PU_PLIC_BASEADDR)
#define PU_PLIC_PRIORITY_BASE  0x000000ul
#define PU_PLIC_ENABLE_BASE    0x002000ul
/* Context 0 = hart 0 M-mode (single-minion demo). */
#define PU_PLIC_THRESHOLD_BASE 0x200000ul
#define PU_PLIC_CLAIM_BASE     0x200004ul
/* 0x80 stride per context for enable words, 0x1000 for threshold/claim. */
#define PU_PLIC_CTX           0u

static inline void plic_set_priority(uint32_t src, uint32_t prio)
{
    mm_mmio_write32(PU_PLIC_BASE + PU_PLIC_PRIORITY_BASE + src * 4U, prio);
}

static inline void plic_enable_src(uint32_t src)
{
    uint32_t word = src / 32U;
    uint32_t bit = src % 32U;
    uintptr_t addr = PU_PLIC_BASE + PU_PLIC_ENABLE_BASE +
                     PU_PLIC_CTX * 0x80U + word * 4U;
    uint32_t v = mm_mmio_read32(addr);
    mm_mmio_write32(addr, v | (1U << bit));
}

static inline void plic_disable_src(uint32_t src)
{
    uint32_t word = src / 32U;
    uint32_t bit = src % 32U;
    uintptr_t addr = PU_PLIC_BASE + PU_PLIC_ENABLE_BASE +
                     PU_PLIC_CTX * 0x80U + word * 4U;
    uint32_t v = mm_mmio_read32(addr);
    mm_mmio_write32(addr, v & ~(1U << bit));
}

static inline void plic_set_threshold(uint32_t thresh)
{
    mm_mmio_write32(PU_PLIC_BASE + PU_PLIC_THRESHOLD_BASE +
                    PU_PLIC_CTX * 0x1000U, thresh);
}

static inline uint32_t plic_claim(void)
{
    return mm_mmio_read32(PU_PLIC_BASE + PU_PLIC_CLAIM_BASE +
                          PU_PLIC_CTX * 0x1000U);
}

static inline void plic_complete(uint32_t src)
{
    mm_mmio_write32(PU_PLIC_BASE + PU_PLIC_CLAIM_BASE +
                    PU_PLIC_CTX * 0x1000U, src);
}

/* ------------------------------------------------------------------ */
/* Timer / UART                                                       */
/* ------------------------------------------------------------------ */

#define MM_MTIMECMP_ADDR ((uintptr_t)(R_PU_RVTIM_BASEADDR + RVTIMER_MTIMECMP_OFFSET))

#define MM_UART_BASE      ((uintptr_t)R_PU_UART_BASEADDR)
#define MM_UART_THR       0x00u
#define MM_UART_LSR       0x14u
#define MM_UART_LSR_THRE  (1u << 5)

static inline void mm_uart_putc(uint8_t c)
{
    while ((mm_mmio_read32(MM_UART_BASE + MM_UART_LSR) & MM_UART_LSR_THRE) == 0U) {
        /* busy wait for THR-empty */
    }
    mm_mmio_write32(MM_UART_BASE + MM_UART_THR, (uint32_t)c);
}

/* ------------------------------------------------------------------ */
/* Upcall state                                                       */
/* ------------------------------------------------------------------ */

/* Set by SYSCALL_OSKERN_REGISTER_IRQ_HANDLER: U-mode entry point that
 * the upcall mret targets.  0 means "no handler registered; spin".
 * Explicitly initialized (MM link policy rejects non-empty .bss). */
uint64_t g_u_irq_handler = 0;

/* Saved U-mode context for one-in-flight upcall.  See layout in the
 * upcall flow comment at the top of this file.  g_u_irq_busy guards
 * against reentry (should not happen with single-source timer IRQ, but
 * we assert loosely by simply dropping nested upcalls). */
struct umode_irq_save_ctx {
    uint64_t regs[32];
    uint64_t mepc;
    uint64_t mstatus;
    uint32_t valid;
};

static struct umode_irq_save_ctx g_u_irq_save = {0};
static volatile uint32_t g_u_irq_busy = 0;

/* ------------------------------------------------------------------ */
/* Handlers for new OSKERN syscalls                                   */
/* ------------------------------------------------------------------ */

static int64_t oskern_irq_enable(uint64_t irq, uint64_t prio)
{
    plic_set_priority((uint32_t)irq, (uint32_t)(prio == 0U ? 1U : prio));
    plic_enable_src((uint32_t)irq);
    /* Ensure context threshold allows our priorities (0 = accept all). */
    plic_set_threshold(0U);
    return SYSCALL_INTERNAL_SUCCESS;
}

static int64_t oskern_irq_disable(uint64_t irq)
{
    plic_disable_src((uint32_t)irq);
    return SYSCALL_INTERNAL_SUCCESS;
}

static int64_t oskern_irq_mask_global(void)
{
    /* MIE is the M-mode global interrupt enable.  U-mode cannot touch it;
     * we do it on its behalf.  Return previous state so unmask can
     * restore. */
    uint64_t prev;
    __asm__ volatile("csrr %0, mstatus" : "=r"(prev));
    __asm__ volatile("csrci mstatus, 0x8");
    return (int64_t)((prev & 0x8U) ? 1 : 0);
}

static int64_t oskern_irq_unmask_global(uint64_t saved)
{
    if (saved) {
        __asm__ volatile("csrsi mstatus, 0x8");
    }
    /* Also ensure Machine External Interrupt is enabled in mie. */
    __asm__ volatile("csrs mie, %0" : : "r"(1UL << MACHINE_EXTERNAL_INTERRUPT));
    return SYSCALL_INTERNAL_SUCCESS;
}

static int64_t oskern_register_irq_handler(uint64_t u_fn_ptr)
{
    g_u_irq_handler = u_fn_ptr;
    /* Prime MIE for M-external so the next IRQ actually traps. */
    __asm__ volatile("csrs mie, %0" : : "r"(1UL << MACHINE_EXTERNAL_INTERRUPT));
    __asm__ volatile("csrsi mstatus, 0x8");
    return SYSCALL_INTERNAL_SUCCESS;
}

static int64_t oskern_set_timeout(uint64_t mtimecmp_abs)
{
    *(volatile uint64_t *)MM_MTIMECMP_ADDR = mtimecmp_abs;
    return SYSCALL_INTERNAL_SUCCESS;
}

static int64_t oskern_console_write(uint64_t ptr, uint64_t len)
{
    const uint8_t *buf = (const uint8_t *)ptr;
    for (uint64_t i = 0; i < len; i++) {
        mm_uart_putc(buf[i]);
    }
    return SYSCALL_INTERNAL_SUCCESS;
}

/* ------------------------------------------------------------------ */
/* IRQ completion: restore the originally-interrupted U-mode context  */
/* into the current (COMPLETE-syscall) trap frame.                    */
/* ------------------------------------------------------------------ */

/* Flag read by trap_handler.S: if set, DON'T add +4 to mepc after
 * returning from syscall_handler_umode (we've already written mepc
 * directly to the saved U PC). */
uint64_t g_u_skip_mepc_bump = 0;

static int64_t oskern_irq_complete(uint64_t *regs, uint64_t irq)
{
    /* PLIC-complete. */
    plic_complete((uint32_t)irq);

    if (!g_u_irq_save.valid) {
        /* Called with no outstanding upcall — treat as no-op. */
        return SYSCALL_INTERNAL_SUCCESS;
    }

    /* Write the saved mepc/mstatus back to the CSRs directly so the
     * trap handler's mret picks them up.  We also need to write the
     * saved regs back into the current trap frame so x1..x31 restore
     * to their pre-upcall values.  sp is tricky: the saved sp is the
     * U-mode sp at the point the IRQ fired.  We want the *restore*
     * path's csrrw sp, mscratch, sp at the end of trap_handler to put
     * the U-mode sp back in sp.  mscratch holds the M-mode sp at entry
     * to the trap handler (our C code sees sp = M-mode sp); mscratch
     * was written with the U-mode sp at trap entry via csrrw.  So we
     * need to write the saved sp into mscratch here. */

    /* Restore regs into the current trap frame (skip x0 and x2; sp
     * goes to mscratch). */
    for (int i = 1; i < 32; i++) {
        if (i == 2) {
            continue;
        }
        regs[i] = g_u_irq_save.regs[i];
    }
    __asm__ volatile("csrw mscratch, %0" : : "r"(g_u_irq_save.regs[2]));
    __asm__ volatile("csrw mepc,     %0" : : "r"(g_u_irq_save.mepc));
    __asm__ volatile("csrw mstatus,  %0" : : "r"(g_u_irq_save.mstatus));

    g_u_irq_save.valid = 0;
    g_u_irq_busy = 0;

    /* Tell trap_handler.S not to bump mepc by 4 (we've already written
     * mepc to the precise resume address). */
    g_u_skip_mepc_bump = 1;

    return SYSCALL_INTERNAL_SUCCESS;
}

/* ------------------------------------------------------------------ */
/* Dispatch: called by syscall_handler_umode for IDs >= 50.           */
/* ------------------------------------------------------------------ */

int64_t oskern_syscall_dispatch(uint64_t number, uint64_t a1, uint64_t a2,
                                 uint64_t a3, uint64_t *regs)
{
    (void)a3;
    switch (number) {
        case SYSCALL_OSKERN_IRQ_ENABLE:
            return oskern_irq_enable(a1, a2);
        case SYSCALL_OSKERN_IRQ_DISABLE:
            return oskern_irq_disable(a1);
        case SYSCALL_OSKERN_IRQ_MASK_GLOBAL:
            return oskern_irq_mask_global();
        case SYSCALL_OSKERN_IRQ_UNMASK_GLOBAL:
            return oskern_irq_unmask_global(a1);
        case SYSCALL_OSKERN_REGISTER_IRQ_HANDLER:
            return oskern_register_irq_handler(a1);
        case SYSCALL_OSKERN_IRQ_COMPLETE:
            return oskern_irq_complete(regs, a1);
        case SYSCALL_OSKERN_SET_TIMEOUT:
            return oskern_set_timeout(a1);
        case SYSCALL_OSKERN_CONSOLE_WRITE:
            return oskern_console_write(a1, a2);
        default:
            return SYSCALL_INTERNAL_INVALID_ID;
    }
}

/* ------------------------------------------------------------------ */
/* M-mode external interrupt -> U-mode upcall                         */
/*                                                                    */
/* Called from trap_handler.S slow path.  regs points to the saved    */
/* x0..x31 array (32 u64 entries).  We:                               */
/*   1. Claim the IRQ from PLIC (no claim => spurious, just return).  */
/*   2. If an upcall is already in flight (g_u_irq_busy), drop.       */
/*   3. Snapshot regs/mepc/mstatus into g_u_irq_save.                 */
/*   4. Overwrite a0 in the frame with irq_num.                       */
/*   5. Point mepc at g_u_irq_handler; keep MPP=U.                    */
/*                                                                    */
/* On return, trap_handler.S restores the modified frame and mrets    */
/* into U-mode at g_u_irq_handler with a0 = irq_num.                  */
/* ------------------------------------------------------------------ */

void mext_interrupt_handler(uint64_t *regs)
{
    uint32_t irq = plic_claim();
    if (irq == 0U) {
        /* Spurious; nothing to do. */
        return;
    }

    if (g_u_irq_busy || g_u_irq_handler == 0U) {
        /* Re-entry or no handler registered: just complete (drop) and
         * return so we don't live-lock.  NOTE: this loses the IRQ from
         * Zephyr's perspective -- MVP-level mitigation. */
        plic_complete(irq);
        return;
    }

    /* Snapshot current U-mode context. */
    for (int i = 0; i < 32; i++) {
        g_u_irq_save.regs[i] = regs[i];
    }
    /* x2/sp is saved to mscratch by trap entry; read it back. */
    uint64_t u_sp;
    __asm__ volatile("csrr %0, mscratch" : "=r"(u_sp));
    g_u_irq_save.regs[2] = u_sp;

    uint64_t mepc_val, mstatus_val;
    __asm__ volatile("csrr %0, mepc"    : "=r"(mepc_val));
    __asm__ volatile("csrr %0, mstatus" : "=r"(mstatus_val));
    g_u_irq_save.mepc = mepc_val;
    g_u_irq_save.mstatus = mstatus_val;
    g_u_irq_save.valid = 1;
    g_u_irq_busy = 1;

    /* Rewrite trap frame: a0 = irq_num.  a0 is x10. */
    regs[10] = (uint64_t)irq;

    /* Point mret at the registered U handler. */
    __asm__ volatile("csrw mepc, %0" : : "r"(g_u_irq_handler));
    /* Keep MPP = U (should already be U since we came from U-mode). */
    /* Ensure MPIE=1 so mret re-enables M-mode interrupts?  Actually
     * M-mode interrupts are ALWAYS enabled when running in U-mode per
     * the RISC-V priv spec, so MPIE doesn't matter for us.  Leave
     * mstatus untouched. */
}
