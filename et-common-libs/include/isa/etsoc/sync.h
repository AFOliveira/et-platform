/***********************************************************************
*
* Copyright (c) 2025 Ainekko, Co.
* SPDX-License-Identifier: Apache-2.0
*
************************************************************************/
/*! \file isa/etsoc/sync.h
    \brief Compatibility forwarder. Content is in isa/common/sync.h and
    uses SEND_FCC / WAIT_FCC from isa/etsoc/fcc.h, pulled in here so the
    common body resolves against the ET-SoC1 ESR path.
*/

#ifndef SYNC_H
#define SYNC_H

#include "isa/etsoc/fcc.h"
#include "isa/etsoc/utils.h"
#include "isa/common/sync.h"

#endif /* SYNC_H */
