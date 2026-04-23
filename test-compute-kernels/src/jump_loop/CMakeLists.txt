# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0

if (TESTKERNELS_USE_GPSDK_RUNTIME)
  test_kernel_with_params(
    NAME jump_loop
    SOURCES jump_loop.c
    SHIM_SOURCE jump_loop_gpsdk_shim.cc
    )
else()
  test_kernel(
    NAME jump_loop
    SOURCES jump_loop.c
    )
endif()
