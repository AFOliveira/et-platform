# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0

if (TESTKERNELS_USE_GPSDK_RUNTIME)
  test_kernel_with_params(
    NAME memset
    SOURCES memset.c
    SHIM_SOURCE memset_gpsdk_shim.cc
    )
else()
  test_kernel(
    NAME memset
    SOURCES memset.c
    )
endif()
