# Copyright (c) 2026 AIFoundry
# SPDX-License-Identifier: Apache-2.0
#
# CMake entry point for the platform-neutral HAL.
#
#   target_link_libraries(foo PRIVATE hal::erbium)     # or hal::etsoc
#
# Or select via HAL_PLATFORM and link hal::platform.

set(HAL_PLATFORM "" CACHE STRING "HAL target platform (erbium | etsoc)")

add_library(hal_api INTERFACE)
target_include_directories(hal_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hal_api INTERFACE c_std_11)

foreach(_plat erbium etsoc)
	set(_plat_dir ${CMAKE_CURRENT_SOURCE_DIR}/platform/${_plat})
	if(NOT EXISTS ${_plat_dir}/api)
		continue()
	endif()

	add_library(hal_${_plat} INTERFACE)
	target_link_libraries(hal_${_plat} INTERFACE hal_api)
	target_include_directories(hal_${_plat} INTERFACE
		${_plat_dir}
		${_plat_dir}/api)
	if(EXISTS ${_plat_dir}/include)
		target_include_directories(hal_${_plat} INTERFACE ${_plat_dir}/include)
	endif()
	add_library(hal::${_plat} ALIAS hal_${_plat})
endforeach()

if(HAL_PLATFORM)
	if(NOT TARGET hal_${HAL_PLATFORM})
		message(FATAL_ERROR
			"HAL_PLATFORM='${HAL_PLATFORM}' has no backend under hal/platform/")
	endif()
	add_library(hal_platform INTERFACE)
	target_link_libraries(hal_platform INTERFACE hal_${HAL_PLATFORM})
	add_library(hal::platform ALIAS hal_platform)
endif()
