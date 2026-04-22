#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common/etsoc/utils.h"

#ifdef NDEBUG
#define assert(condition) ((void)0)
#else
#define assert(condition) et_assert(condition)
#endif

#ifdef __cplusplus
}
#endif