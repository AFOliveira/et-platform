#ifndef SYSCALL_H
#define SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif
/* TODO: Legacy definitions to be removed */
// From U-mode to S-mode for compute firmware [300-399]
#define SYSCALL_CACHE_CONTROL 301
#define SYSCALL_FLUSH_L3      302
#define SYSCALL_SAMPLE_PMCS   309
#define SYSCALL_RESET_PMCS    310

/* SYSCALL IDs for syscalls from U-Mode */
/* Range (0-127) dedicated for syscalls allowed from U-Mode */
#define SYSCALL_UMODE_THRESHOLD             0
#define SYSCALL_CACHE_OPS_EVICT_SW          (SYSCALL_UMODE_THRESHOLD + 1)
#define SYSCALL_CACHE_OPS_FLUSH_SW          (SYSCALL_UMODE_THRESHOLD + 2)
#define SYSCALL_CACHE_OPS_LOCK_SW           (SYSCALL_UMODE_THRESHOLD + 3)
#define SYSCALL_CACHE_OPS_UNLOCK_SW         (SYSCALL_UMODE_THRESHOLD + 4)
#define SYSCALL_CACHE_OPS_INVALIDATE        (SYSCALL_UMODE_THRESHOLD + 5)
#define SYSCALL_CACHE_OPS_EVICT_L1          (SYSCALL_UMODE_THRESHOLD + 6)
#define SYSCALL_SHIRE_CACHE_BANK_OP         (SYSCALL_UMODE_THRESHOLD + 7)
#define SYSCALL_RETURN_FROM_KERNEL          (SYSCALL_UMODE_THRESHOLD + 8)
#define SYSCALL_PMC_SC_SAMPLE               (SYSCALL_UMODE_THRESHOLD + 9)
#define SYSCALL_PMC_MS_SAMPLE               (SYSCALL_UMODE_THRESHOLD + 10)
#define SYSCALL_CACHE_OPS_EVICT_WHOLE_L1_L2 (SYSCALL_UMODE_THRESHOLD + 11)

/* OS-kernel syscalls (50..127): provided to U-mode kernels that need a
 * minimal privileged surface (IRQ mux, timer arming, console I/O).  These
 * are distinct from the compute-kernel IDs 1..11 above so a U-mode OS can
 * coexist with compute kernels.  Implemented by MachineMinion in the
 * direct-M-from-U build (feat/machineminion-irq-upcall).
 *
 *   IRQ_ENABLE          a1=irq_num, a2=priority
 *   IRQ_DISABLE         a1=irq_num
 *   IRQ_MASK_GLOBAL     no args; returns previous mstatus.MIE state
 *   IRQ_UNMASK_GLOBAL   a1=saved state
 *   REGISTER_IRQ_HANDLER a1=u_fn_ptr (U-mode entry for IRQ upcall)
 *   IRQ_COMPLETE        a1=irq_num; returns from IRQ upcall to interrupted U ctx
 *   SET_TIMEOUT         a1=absolute mtimecmp value
 *   CONSOLE_WRITE       a1=ptr, a2=len (byte buffer, host UART)
 */
#define SYSCALL_OSKERN_IRQ_ENABLE           (SYSCALL_UMODE_THRESHOLD + 50)
#define SYSCALL_OSKERN_IRQ_DISABLE          (SYSCALL_UMODE_THRESHOLD + 51)
#define SYSCALL_OSKERN_IRQ_MASK_GLOBAL      (SYSCALL_UMODE_THRESHOLD + 52)
#define SYSCALL_OSKERN_IRQ_UNMASK_GLOBAL    (SYSCALL_UMODE_THRESHOLD + 53)
#define SYSCALL_OSKERN_REGISTER_IRQ_HANDLER (SYSCALL_UMODE_THRESHOLD + 54)
#define SYSCALL_OSKERN_IRQ_COMPLETE         (SYSCALL_UMODE_THRESHOLD + 55)
#define SYSCALL_OSKERN_SET_TIMEOUT          (SYSCALL_UMODE_THRESHOLD + 56)
#define SYSCALL_OSKERN_CONSOLE_WRITE        (SYSCALL_UMODE_THRESHOLD + 57)

#define SYSCALL_UMODE_THRESHOLD_LIMIT       127

/* SYSCALL IDs for syscalls from U-Mode */
/* Range (128-511) dedicated for syscalls allowed from S-Mode */
#define SYSCALL_SMODE_THRESHOLD       128
#define SYSCALL_SMODE_THRESHOLD_LIMIT 512

/* SYSCALL error codes */
#define SYSCALL_SUCCESS    0
#define SYSCALL_INVALID_ID -1

/* Kernel return types. Must be kept synced with FW.
TODO: Need a separate header for it? */
#define KERNEL_RETURN_SUCCESS      0
#define KERNEL_RETURN_SELF_ABORT   1
#define KERNEL_RETURN_SYSTEM_ABORT 2
#define KERNEL_RETURN_EXCEPTION    3

#ifndef __ASSEMBLER__

#include <stdint.h>

static inline __attribute__((always_inline)) int64_t syscall(
    uint64_t syscall, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    register uint64_t a0 asm("a0") = syscall;
    register uint64_t a1 asm("a1") = arg1;
    register uint64_t a2 asm("a2") = arg2;
    register uint64_t a3 asm("a3") = arg3;

    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3) : "memory");

    return (int64_t)a0;
}

#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif

#endif // SYSCALL_H
