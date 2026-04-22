/***********************************************************************/
/*! \copyright
  Copyright (c) 2025 Ainekko, Co.
  SPDX-License-Identifier: Apache-2.0
*/
/***********************************************************************/
/*! \file isa/etsoc/barriers.h
    \brief Compatibility forwarder. Content now lives in
    isa/common/barriers.h (platform-neutral; uses fcc_send /
    flbarrier_set from isa/etsoc/{fcc,flb}.h).
*/
/***********************************************************************/

#ifndef _BARRIERS_H_
#define _BARRIERS_H_

#include "isa/etsoc/fcc.h"
#include "isa/etsoc/flb.h"
#include "isa/common/barriers.h"

#endif /* _BARRIERS_H_ */
