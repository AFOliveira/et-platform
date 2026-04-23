# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.15)

# Opt-in: link test kernels through gp-sdk's halified device runtime
# (crt0.S + deviceGpSdkEntry + linker.ld) instead of the self-contained
# shared/crt.S + shared/sections.ld. When OFF (default) the build is
# byte-identical to the legacy flow; when ON we need C++ support for
# deviceGpSdk.cc and we pull gp-sdk/device as a subdirectory.
option(TESTKERNELS_USE_GPSDK_RUNTIME "Link test kernels through gp-sdk device runtime" OFF)

if (TESTKERNELS_USE_GPSDK_RUNTIME)
  project(EsperantoTestKernels VERSION 1.14.0 LANGUAGES C CXX ASM)
else()
  project(EsperantoTestKernels VERSION 1.14.0 LANGUAGES C ASM)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-zero-initialized-in-bss -ffunction-sections -fdata-sections ")
if (NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

include(ExternalProject)
include(GNUInstallDirs)
include(DeviceUtils)
include(EsperantoTestKernelsUtils)

cmake_policy(SET CMP0076 NEW)

# Dependencies
find_package(esperantoTrace REQUIRED)
find_package(et-common-libs REQUIRED COMPONENTS cm-umode)

# When TESTKERNELS_USE_GPSDK_RUNTIME is ON we pull in gp-sdk/device so the
# halified etsoc_crt0 + add_etsoc_riscv_executable helper become available.
# GPSDK_DEVICE_DIR points at the et-platform/gp-sdk/device source tree; the
# default co-located layout (../gp-sdk/device relative to this tree) works
# when test-compute-kernels lives inside et-platform.
if (TESTKERNELS_USE_GPSDK_RUNTIME)
    set(GPSDK_DEVICE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../gp-sdk/device"
        CACHE PATH "Path to gp-sdk/device source tree")
    if (NOT EXISTS "${GPSDK_DEVICE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "TESTKERNELS_USE_GPSDK_RUNTIME=ON but GPSDK_DEVICE_DIR='${GPSDK_DEVICE_DIR}' "
            "does not contain a CMakeLists.txt. Pass -DGPSDK_DEVICE_DIR=<path>.")
    endif()
    # ET_PLATFORM must match what the gp-sdk runtime was configured for.
    if (NOT DEFINED ET_PLATFORM)
        set(ET_PLATFORM "etsoc" CACHE STRING "Target SoC platform (etsoc, erbium)")
    endif()
    # gp-sdk/device/tests is ET-SoC1-only and also compiles fcc.h-heavy kernels
    # that we do not want building here; just reuse the runtime targets.
    set(BUILD_TESTS OFF CACHE BOOL "Disable gp-sdk in-tree kernels from this nested build" FORCE)
    # ADDRESS is consumed by gp-sdk's add_etsoc_riscv_executable when it is
    # called from this tree. gp-sdk/device/CMakeLists.txt sets a default via
    # `set(ADDRESS ...)` (non-cache), so the value only lives in the nested
    # subdirectory scope. Promote it into the cache here so test-compute-
    # kernels' own targets see the same value when they invoke the helper.
    if (NOT DEFINED CACHE{ADDRESS})
        if (ET_PLATFORM STREQUAL "erbium")
            set(ADDRESS "0x40010000" CACHE STRING "Kernel link address")
        else()
            set(ADDRESS "0x8006335000" CACHE STRING "Kernel link address")
        endif()
    endif()
    add_subdirectory(${GPSDK_DEVICE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/_gpsdk_device)
    message(STATUS "test-compute-kernels: using gp-sdk device runtime (ET_PLATFORM=${ET_PLATFORM}) ADDRESS=${ADDRESS}")
endif()


set(BIN_INSTALL_DIR bin CACHE STRING "Bin install dir")
set(INCLUDE_INSTALL_DIR include/ CACHE STRING "Include install dir")
set(LIB_INSTALL_DIR  lib/  CACHE STRING "Library install dir")
set(SYSCONFIG_INSTALL_DIR etc/et-fw/ CACHE STRING "Config install dir")

### project contents (sources, tools, tests, etc)

add_subdirectory(src)


### Installation

include(CMakePackageConfigHelpers)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE EsperantoTestKernelsTargets)

# Create the *-config.cmake and *-config-version.cmake files
configure_package_config_file(EsperantoTestKernelsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EsperantoTestKernelsConfig.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/EsperantoTestKernels/cmake
    PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR LIB_INSTALL_DIR BIN_INSTALL_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EsperantoTestKernelsConfigVersion.cmake
    VERSION ${VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the *Config.cmake and *ConfigVersion.cmake
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EsperantoTestKernelsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EsperantoTestKernelsConfigVersion.cmake
    DESTINATION ${LIB_INSTALL_DIR}/cmake/EsperantoTestKernels
    COMPONENT machine-minion
)

# Install the export set for use with the install-tree. When the gp-sdk
# runtime is in use the kernels have no exported targets (etsoc_crt0 is not
# in this project's export set), so skip this step.
if (NOT TESTKERNELS_USE_GPSDK_RUNTIME)
    install(EXPORT EsperantoTestKernelsTargets
        NAMESPACE esperanto-test-kernels::
        DESTINATION ${LIB_INSTALL_DIR}/cmake/EsperantoTestKernels
    )
endif()
