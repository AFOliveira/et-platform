########################
#Compute Minion User Mode
########################

set(CM_UMODE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/cm-umode)

################################################
# List the public interfaces and headers to be
#exposed to Compute Minion User Mode here
################################################

#Listing of header only public interfaces
set(CM_UMODE_HDRS
    # ETSOC ISA headers
    include/isa/common/atomic.h
    include/isa/common/atomic-impl.h
    include/isa/common/barriers.h
    include/isa/etsoc/barriers.h
    include/isa/etsoc/cacheops.h
    include/isa/common/cacheops.h
    include/isa/common/cacheops_common.h
    include/isa/etsoc/cacheops-umode.h    
    include/isa/common/cacheops-umode.h
    include/isa/etsoc/esr_defines.h
    include/isa/etsoc/fcc.h
    include/isa/etsoc/flb.h
    include/isa/common/hart.h
    include/isa/etsoc/sync.h
    include/isa/common/sync.h
    include/isa/common/syscall.h
    include/isa/common/tensors.h
    include/isa/etsoc/utils.h
    include/isa/common/utils.h
    # Register-access + ESR + memory-map foundation
    include/isa/common/mmio.h
    include/isa/common/csr.h
    include/isa/common/esr.h
    include/isa/common/memmap.h
    include/isa/etsoc/esr_platform.h
    include/isa/etsoc/memmap_platform.h
    include/isa/erbium/esr_platform.h
    include/isa/erbium/memmap_platform.h
    # Erbium platform slice
    include/isa/erbium/esr_defines.h
    include/isa/erbium/flb.h
    include/isa/erbium/fcc.h
    include/isa/erbium/utils.h
    include/isa/erbium/barriers.h
    include/isa/erbium/cacheops-umode.h
    include/isa/erbium/cacheops.h
    include/isa/erbium/sync.h
    include/trace/trace_umode.h
    include/trace/trace_umode_cb.h
    include/system/abi.h
)

#Listing of public headers that expose services provided by
#the CM UMODE (Compute Minion User Mode) Library
set(CM_UMODE_LIB_HDRS
    include/common/etsoc/utils.h
    include/drivers/etsoc/pmu/pmu.h
)

########################
#Create cm-umode library
########################
#Listing of sources that implement services provided by
#the CM UMODE (Compute Minion User Mode) Library
add_library(cm-umode STATIC
    src/common/printf.c
    src/common/etsoc/utils.c
    src/drivers/etsoc/pmu/pmu.c
    src/trace/trace_umode.c
    src/common/printf_dummy.c
    src/libc_stub/stdlib.c
    src/libc_stub/string.c
)
add_library(et-common-libs::cm-umode ALIAS cm-umode)
target_include_directories(cm-umode
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CM_UMODE_INSTALL_PREFIX}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/libc_stub
)

target_compile_features(cm-umode PUBLIC c_std_11)

if (ET_PLATFORM STREQUAL "erbium")
    target_compile_definitions(cm-umode PUBLIC ET_PLATFORM_ERBIUM=1)
    # The Erbium ISA headers (isa/erbium/*.h) reference hwinc/top.h and
    # hwinc/esr.h, which are produced by the Erbium HAL RDL generator and
    # live outside et-common-libs. Make those headers visible to cm-umode
    # users (gp-sdk kernels etc.) by bundling them into the install tree.
    set(ERBIUM_HWINC_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../hal/platform/erbium/include/hwinc")
    if (EXISTS "${ERBIUM_HWINC_SRC_DIR}/top.h")
        target_include_directories(cm-umode PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../hal/platform/erbium/include>
            $<INSTALL_INTERFACE:${CM_UMODE_INSTALL_PREFIX}/include>)
        install(DIRECTORY "${ERBIUM_HWINC_SRC_DIR}"
                DESTINATION ${CM_UMODE_INSTALL_PREFIX}/include)
    else()
        message(FATAL_ERROR "Erbium hwinc not found at ${ERBIUM_HWINC_SRC_DIR}. "
                "Build ET_PLATFORM=erbium requires the Erbium HAL sources.")
    endif()
endif()

target_compile_options(cm-umode
    PRIVATE
        -Wall
        $<$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>:-Werror>
)
target_link_libraries(cm-umode
    PUBLIC
        esperantoTrace::et_trace
)

#This macro preserves the driectory structure as defined by the
#CM UMODE listing above
macro(InstallHdrsWithDirStruct HEADER_LIST)
    foreach(HEADER ${${HEADER_LIST}})
        string(REGEX MATCH "(.*)[/\]" DIR ${HEADER})
        install(FILES ${HEADER} DESTINATION ${CM_UMODE_INSTALL_PREFIX}/${DIR})
    endforeach(HEADER)
endmacro(InstallHdrsWithDirStruct)

InstallHdrsWithDirStruct(CM_UMODE_HDRS)
InstallHdrsWithDirStruct(CM_UMODE_LIB_HDRS)

################################################
#Install and export cm-umode library and headers
################################################

install(
    TARGETS cm-umode
    EXPORT cm-umodeTargets
    LIBRARY DESTINATION ${CM_UMODE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CM_UMODE_INSTALL_PREFIX}/lib
    INCLUDES DESTINATION ${CM_UMODE_INSTALL_PREFIX}/include
)

#TODO: Could be improved and made more flexible by exporting a package
install(
    EXPORT cm-umodeTargets
    NAMESPACE et-common-libs::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/et-common-libs/cm-umode
    COMPONENT cm-umode
)

