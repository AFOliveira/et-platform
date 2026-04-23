/***********************************************************************/
/*! \copyright
  Copyright (c) 2025 Ainekko, Co.
  SPDX-License-Identifier: Apache-2.0
*/
/***********************************************************************/
/*! \file include/common/etsoc/utils.h
    \brief A C header that defines the common util routines to be used.
*/
/***********************************************************************/

#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "drivers/etsoc/pmu/pmu.h"

/*
 * Trace backend selection.
 *
 * On ET-SoC1 (default): pull in the full et-trace umode surface. et_printf
 * formats into the per-hart trace control block living in DDR.
 *
 * On Erbium (ET_PLATFORM_ERBIUM): there is no trace control block yet --
 * neither the per-hart CB array nor the trace buffer is laid out in MRAM,
 * and nothing in Zephyr-M mirrors them back to the host. Including
 * trace/trace_umode.h on Erbium would drag in CM_UMODE_TRACE_CB whose
 * hardcoded ET-SoC1 DDR base address (0x8004F23000) does not exist in
 * Erbium's memory map, causing load-access-faults the moment any kernel
 * calls et_printf / Trace_Format_String / trace_is_enabled.
 *
 * Until a real Erbium trace backend lands, compile et_printf and friends
 * into no-ops. Kernels that use log/beef/trace helpers then still build
 * and run -- they just produce no trace output.
 */
#ifndef ET_PLATFORM_ERBIUM
#include "trace/trace_umode.h"
#endif

/*! \def et_printf(fmt, ...)
    \brief Write a log with va_list style args
    \param fmt String format specifier
    \param ... Variable argument list
    \return none
    \stdlib Esperanto specific implementation of printf.
    \example et_printf.c
    This is an example of using et_printf api
*/
#ifdef ET_PLATFORM_ERBIUM
/* No trace backend on Erbium yet; route to a variadic no-op helper that
 * both silences -Wunused-parameter on callers whose formals only exist
 * to feed et_printf (e.g. __assert_func's file/line/function/expr) and
 * gets optimized away entirely. */
static inline void et_printf_noop(const char *fmt, ...)
{
    (void)fmt;
}
#define et_printf(fmt, ...) et_printf_noop((fmt), ##__VA_ARGS__)
#else
#define et_printf(fmt, ...)                          \
    Trace_Format_String(TRACE_EVENT_STRING_CRITICAL, \
        &CM_UMODE_TRACE_CB[GET_CB_INDEX(get_hart_id())].cb, fmt, ##__VA_ARGS__)
#endif

/*! \fn void *et_memset(void *s, int c, size_t n)
    \brief Copies the character c to the first n characters of the string pointed argument s.
    \param s Pointer to memory control block
    \param c The value to be set
    \param n Number of bytes
    \return Pointer to memory area s
    \stdlib Esperanto specific implementation of memset
    \example et_memset.c
    This is an example of et_memset api usage.
*/
void *et_memset(void *s, int c, size_t n);

/*! \fn void *et_memcpy(void *dest, const void *src, size_t n)
    \brief Copies n characters from memory area src to memory area dest.
    \param dest This is pointer to the destination buffer
    \param src This is pointer to the source buffer
    \param n Number of bytes
    \return Pointer to destination buffer
    \stdlib Esperanto specific implementation of memcpy
    \example et_memcpy.c
    This is an example of et_memcpy api usage.
*/
void *et_memcpy(void *dest, const void *src, size_t n);

/*! \fn int et_memcmp(const void *s1, const void *s2, size_t n)
    \brief Compares the first n bytes of memory area s1 and memory area s2.
    \param s1 This is pointer to a buffer
    \param s2 This is pointer to a buffer
    \param n Number of bytes to compare
    \return < 0 then it indicates s1 is less than s2, > 0 then it indicates s2
    is less than s1, = 0 then it indicates s1 is equal to s2
    \stdlib Esperanto specific implementation of memcmp
    \example et_memcmp.c
    This is an example of et_memcmp api usage.
*/
int et_memcmp(const void *s1, const void *s2, size_t n);

/*! \fn size_t et_strlen(const char *str)
    \brief Computes the length of the string str up to, but not including the
    terminating null character.
    \param str String whos length needs to be found
    \return Length of string
    \stdlib Esperanto specific implementation of strlen
    \example et_strlen.c
    This is an example of et_strlen api usage.
*/
size_t et_strlen(const char *str);

/*! \fn void et_abort(void)
    \brief Causes an abnormal end of the application and returns from kernel launch.
    \return None
    \stdlib Implementation of kernel abort api
    \example et_abort.c
    This is an example of et_abort api usage.
*/
__attribute__((noreturn)) void et_abort(void);

/*! \def et_assert(expr)
    \brief A macro that allows diagnostic information to be written to the trace buffer
    and abort the application if the expr provided is false.
    \stdlib Implementation of et_asset api
    \example et_assert.c
    This is an example of et_assert api usage.
*/
#define et_assert(expr)                                                                      \
    if (!(expr))                                                                             \
    {                                                                                        \
        et_printf("Assertion \"%s\" failed: file \"%s\", line %d%s%s\n", #expr, __FILE__,    \
            __LINE__, __FUNCTION__ ? ", function: " : "", __FUNCTION__ ? __FUNCTION__ : ""); \
        et_abort();                                                                          \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
        (void)(expr);                                                                        \
    }

/*! \def et_get_timestamp()
    \brief A macro that returns current timestamp in terms of cycles.
    \stdlib Implementation of et_get_timestamp api
    \example et_get_timestamp.c
    This is an example of et_get_timestamp api usage.
*/
#define et_get_timestamp() PMC_Get_Current_Cycles()

/*! \def et_get_delta_timestamp(prev_timestamp)
    \brief A define to get difference between timestamp cycles.
    \stdlib Implementation of et_get_delta_timestamp api
    \example et_get_delta_timestamp.c
    This is an example of et_get_delta_timestamp api usage.
*/
#define et_get_delta_timestamp(prev_timestamp) (PMC_Get_Current_Cycles() - prev_timestamp)

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_UTILS_H_ */
