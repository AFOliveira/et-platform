# Copyright (c) 2026 AIFoundry
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_HAS_AIFOUNDRY_HAL)
  # Generic, OS-agnostic HAL — used by tests and any consumer.
  zephyr_include_directories(include)
  # Zephyr-only glue (e.g., aifoundry/cache.h for arch_dcache_*).
  zephyr_include_directories(zephyr/include)
  # Erbium HAL is header-only — no .c files to compile.
endif()
