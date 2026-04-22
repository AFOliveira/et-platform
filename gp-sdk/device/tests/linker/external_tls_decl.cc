/*-------------------------------------------------------------------------
 * Copyright (c) 2025 Ainekko, Co.
 * SPDX-License-Identifier: Apache-2.0
 *-------------------------------------------------------------------------
 */


#include <common/etsoc/utils.h>
#include <isa/etsoc/hart.h>

extern const thread_local uint32_t  * testTlsPtr;

void testExternalTlsAccess();
void testExternalTlsAccess() {
  et_printf("%x %x ", testTlsPtr[0], testTlsPtr[1]);
}
