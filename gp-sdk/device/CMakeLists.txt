#------------------------------------------------------------------------------
# Copyright (c) 2025 Ainekko, Co.
# SPDX-License-Identifier: Apache-2.0
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(gp-sdk-device VERSION 1.6.0 LANGUAGES C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_DOCS "Enable/disable building documentation" OFF)
option(BUILD_TESTS "Build GP-SDK test kernels" ON)
set(GP_SDK_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../tools" CACHE PATH "Path to GP-SDK tools")
set(GP_SDK_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/linker.ld" CACHE PATH "Path to device linker script")

# Target SoC platform. Mirrors et-common-libs' ET_PLATFORM option so gp-sdk
# kernels and runtime can be built for ET-SoC1 or Erbium from the same tree.
set(ET_PLATFORM "etsoc" CACHE STRING "Target SoC platform (etsoc, erbium)")
set_property(CACHE ET_PLATFORM PROPERTY STRINGS etsoc erbium)
if (NOT ET_PLATFORM STREQUAL "etsoc" AND NOT ET_PLATFORM STREQUAL "erbium")
    message(FATAL_ERROR "ET_PLATFORM must be 'etsoc' or 'erbium' (got '${ET_PLATFORM}')")
endif()
message(STATUS "gp-sdk-device ET_PLATFORM = ${ET_PLATFORM}")

if (ET_PLATFORM STREQUAL "erbium")
    add_compile_definitions(ET_PLATFORM_ERBIUM=1)
else()
    add_compile_definitions(ET_PLATFORM_ETSOC=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Configuring Device build for clang Compiler.. ")
  string(APPEND CMAKE_ASM_FLAGS " ${CMAKE_C_FLAGS}")
else()
  message(STATUS "Configuring Device build for gcc Compiler.. ")
endif()
 

if (NOT ADDRESS)
  # Default kernel link address per platform so typical consumers don't have
  # to supply ADDRESS explicitly. ET-SoC1 uses the DDR kernel U-mode entry
  # that MachineMinion loads kernels to; Erbium uses an MRAM offset agreed
  # with the Zephyr-M launcher (see umode_gpsdk sample: -file_load 0x40010000).
  if (ET_PLATFORM STREQUAL "erbium")
    set(ADDRESS "0x40010000")
  else()
    set(ADDRESS "0x8006335000")
  endif()
  message(STATUS "ADDRESS not provided; defaulting to ${ADDRESS} for ${ET_PLATFORM}")
endif()

set(CMAKE_CXX_STANDARD 17)

find_package(et-common-libs REQUIRED COMPONENTS cm-umode)
find_package(dnnLibrary)

include("${CMAKE_CURRENT_SOURCE_DIR}/riscv_helpers.cmake")

## project documentation
if (BUILD_DOCS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/../docs/device ${CMAKE_SOURCE_DIR}/../docs/device/build)
endif()

add_subdirectory(sdk)
if (BUILD_TESTS)
  # The in-tree gp-sdk test kernels are ET-SoC1-only: many of them directly
  # #include <isa/etsoc/fcc.h>, use ESR_SHIRE broadcast helpers, or assume
  # multi-shire topology. Erbium consumers drive their own test corpus via
  # CUSTOM_KERNELS_{SRC,BIN}_DIR (e.g. test-compute-kernels).
  if (ET_PLATFORM STREQUAL "etsoc")
    add_subdirectory(tests)
  else()
    message(STATUS "Skipping gp-sdk/device/tests (ET-SoC1-specific) for ET_PLATFORM=${ET_PLATFORM}")
  endif()
endif()

# Halify acceptance probes: minimal kernels that exercise the halified
# deviceGpSdkEntry / crt0 / linker.ld on both ET_PLATFORM values.
add_subdirectory(halify-tests)

# Users of this CMakeLists.txt can provide an additional subdirectory by passing build arguments,
# as opposed to modifying this CMakeLists.txt. This CMake configuration mechanism is intended for
# consumers of the GP-SDK to build custom kernels leveraging the same build recipe from the
# internal GP-SDK tests. The accepted parameters are:
#
# - CUSTOM_KERNELS_SRC_DIR: directory where the sources and the corresponding CMakeLists.txt
#   are located. It can be located out of this tree.
#
# - CUSTOM_KERNELS_BIN_DIR: directory where the binary files will be stored. If the directory
#   does not exist it gets created.
#
# If one such pair of arguments is provided it is handled at this stage.

if (CUSTOM_KERNELS_SRC_DIR AND CUSTOM_KERNELS_BIN_DIR)
    add_subdirectory(${CUSTOM_KERNELS_SRC_DIR} ${CUSTOM_KERNELS_BIN_DIR})
endif()

# Users can also hack their way by just editing this file and have their own project added as a
# subdirectory here without any extra arguments being passed. That build method is suggested in
# the tutorial from the GP-SDK user guide but has also been labeled as not ideal for production by
# some users of the GP-SDK.
#
# add_subdirectory(tutorial_example)
