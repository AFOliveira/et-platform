# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0

if (TESTKERNELS_USE_GPSDK_RUNTIME)
  test_kernel_with_params(
    NAME crc32
    SOURCES crc32.c
    SHIM_SOURCE crc32_gpsdk_shim.cc
    INCLUDES include ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
    )
else()
  test_kernel(
    NAME crc32
    SOURCES crc32.c
    INCLUDES include
    )
endif()
