/***********************************************************************
 *
 * Copyright (c) 2026 AIFoundry
 * SPDX-License-Identifier: Apache-2.0
 *
 ***********************************************************************/
/*! \file isa/erbium/sync.h
 *  \brief Forwarder to isa/common/sync.h with Erbium fcc/utils in scope.
 *
 *  sync.h's FCC-based sync primitives resolve SEND_FCC / WAIT_FCC to the
 *  Erbium USER_CPU.CREDINC path via isa/erbium/fcc.h. The `shire` argument
 *  to SEND_FCC is accepted for source compatibility and ignored.
 */

#ifndef _ISA_ERBIUM_SYNC_H_
#define _ISA_ERBIUM_SYNC_H_

#include "isa/erbium/fcc.h"
#include "isa/erbium/utils.h"
#include "isa/common/sync.h"

#endif /* _ISA_ERBIUM_SYNC_H_ */
