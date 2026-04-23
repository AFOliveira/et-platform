#------------------------------------------------------------------------------
# Copyright (c) 2026 AIFoundry
# SPDX-License-Identifier: Apache-2.0
#
# Minimal gp-sdk halify verification kernels. Each kernel mirrors a
# corresponding source under test-compute-kernels/src/ but wraps the entry
# point with DECLARE_KERNEL_ENTRY_POINTS so gp-sdk's crt0 + deviceGpSdkEntry
# run it. These are the acceptance probes for the halification work: they
# must build and link cleanly for both ET_PLATFORM=etsoc and =erbium.
#------------------------------------------------------------------------------

function(add_halify_kernel TARGET_NAME SOURCE_FILE)
  add_etsoc_riscv_executable(${TARGET_NAME} ${SOURCE_FILE})
  target_link_libraries(${TARGET_NAME} etsoc_crt0)
endfunction()

add_halify_kernel(halify_abort.elf       abort.cc)
add_halify_kernel(halify_empty.elf       empty.cc)
add_halify_kernel(halify_log.elf         log.cc)
add_halify_kernel(halify_add_vector.elf  add_vector.cc)
