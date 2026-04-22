/*
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 * Platform-independent poll-mode UART access. Backend selected via
 * ET_PLATFORM_ERBIUM. Surface:
 *
 *   uart_supports_pinmux_gate, uart_enable_pinmux
 *   uart_supports_baud_roundtrip, uart_baud_get, uart_baud_set
 *   uart_tx_ready, uart_tx_empty, uart_rx_ready
 *   uart_rx_byte, uart_tx_byte
 *
 * Backends normalize their per-IP register layout (DesignWare on ET-SoC1,
 * Shakti on Erbium) to this common surface. The `_supports_*` predicates
 * let portable code detect features the current platform doesn't expose
 * (ET-SoC1 has no pinmux gate or programmable baud via this interface).
 */

#ifndef _DRIVERS_COMMON_UART_H_
#define _DRIVERS_COMMON_UART_H_

#ifdef ET_PLATFORM_ERBIUM
#include "drivers/erbium/uart_platform.h"
#else
#include "drivers/etsoc/uart_platform.h"
#endif

#endif /* _DRIVERS_COMMON_UART_H_ */
