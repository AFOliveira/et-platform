/*-------------------------------------------------------------------------
* Copyright (c) 2026 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*-------------------------------------------------------------------------*/

#include "emu_defines.h"

#if EMU_ERBIUM
#include "boot-protocol_er.cpp"
#elif EMU_ETSOC1
#include "boot-protocol_et.cpp"
#else
#error "Architecture not supported"
#endif
