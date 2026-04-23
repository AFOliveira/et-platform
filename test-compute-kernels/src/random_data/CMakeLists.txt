# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0

if (TESTKERNELS_USE_GPSDK_RUNTIME)
  test_kernel_with_params(
    NAME random_data
    SOURCES random_data.c
    SHIM_SOURCE random_data_gpsdk_shim.cc
    INCLUDES include ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
    )
else()
  test_kernel(
    NAME random_data
    SOURCES random_data.c
    INCLUDES include
    )
endif()
