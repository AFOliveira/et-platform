# Copyright (c) 2026 AIFoundry
# SPDX-License-Identifier: Apache-2.0
#
# Make entry point for the platform-neutral HAL.
#
#   HAL_ROOT     := <path-to>/hal
#   HAL_PLATFORM := erbium
#   include $(HAL_ROOT)/platform.mk
#   CPPFLAGS += $(HAL_CPPFLAGS)

ifndef HAL_PLATFORM
$(error HAL_PLATFORM not set (expected: erbium | etsoc))
endif

HAL_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))
HAL_PLATFORM_DIR := $(HAL_ROOT)/platform/$(HAL_PLATFORM)

ifeq ($(wildcard $(HAL_PLATFORM_DIR)/api),)
$(error HAL_PLATFORM='$(HAL_PLATFORM)' has no backend at $(HAL_PLATFORM_DIR)/api)
endif

HAL_CPPFLAGS := -I$(HAL_ROOT) \
                -I$(HAL_PLATFORM_DIR) \
                -I$(HAL_PLATFORM_DIR)/api

ifneq ($(wildcard $(HAL_PLATFORM_DIR)/include),)
HAL_CPPFLAGS += -I$(HAL_PLATFORM_DIR)/include
endif
