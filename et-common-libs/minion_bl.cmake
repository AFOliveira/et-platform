########################
# Minion Boot Loader
########################

#Minion Boot Loader requires etsoc_hal library
find_package(etsoc_hal REQUIRED)

#Install prefix for Minion Bootloader library
set(MINION_BL_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/minion-bl)

################################################
# List the public interfaces and headers to be
# exposed to Minion Boot Loader
################################################

#Listing of header only public interfaces
set(MINION_BL_HDRS
    include/drivers/etsoc/pmu/pmu.h
    include/isa/common/atomic.h
    include/isa/common/atomic-impl.h
    include/isa/etsoc/etsoc_memory.h
    include/isa/etsoc/esr_defines.h
    include/isa/common/macros.h
    include/isa/etsoc/fcc.h
    include/isa/etsoc/flb.h
    include/isa/common/hart.h
    include/isa/common/io.h
    include/isa/etsoc/sync.h
    include/isa/common/sync.h
    include/isa/etsoc/cacheops.h
    include/isa/common/cacheops.h
    include/isa/common/cacheops_common.h
    include/isa/common/syscall.h
    include/isa/common/riscv_encoding.h
    include/isa/etsoc/utils.h
    include/isa/common/utils.h
    include/system/etsoc_ddr_region_map.h
    include/system/layout.h
)

#Listing of public headers that expose services provided by
#the MINION_BL (Minion Bootloader Library
set(MINION_BL_LIB_HDRS
    include/transports/mm_cm_iface/broadcast.h
)

#########################
#Create minion-bl library
#########################

#Listing of sources that implement services provided by
#the MINION_BL (Minion Bootloader) Library
add_library(minion-bl STATIC
    src/drivers/etsoc/pmu/pmu.c
    src/transports/mm_cm_iface/broadcast.c
)

set_target_properties(minion-bl PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(minion-bl PUBLIC etsoc_hal::etsoc_hal)

target_include_directories(minion-bl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${MINION_BL_INSTALL_PREFIX}/include>
)

target_compile_features(minion-bl PUBLIC c_std_11)

target_compile_options(minion-bl
    PRIVATE
        -Wall
        -fno-strict-aliasing
        $<$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>:-Werror>
)

#################################################
#Install and export minion-bl library and headers
#################################################

#This macro preserves the driectory structure as defined by the
#MINION_BL listing above
MACRO(InstallHdrsWithDirStruct HEADER_LIST)
    FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/\]" DIR ${HEADER})
    INSTALL(FILES ${HEADER} DESTINATION ${MINION_BL_INSTALL_PREFIX}/${DIR})
    ENDFOREACH(HEADER)
ENDMACRO(InstallHdrsWithDirStruct)

InstallHdrsWithDirStruct(MINION_BL_HDRS)
InstallHdrsWithDirStruct(MINION_BL_LIB_HDRS)

install(
    TARGETS minion-bl
    EXPORT minion-blTargets
    LIBRARY DESTINATION ${MINION_BL_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${MINION_BL_INSTALL_PREFIX}/lib
    INCLUDES DESTINATION ${MINION_BL_INSTALL_PREFIX}/include
)

#TODO: Could be improved and made more flexible by exporting a package
install(
    EXPORT minion-blTargets
    NAMESPACE et-common-libs::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/et-common-libs/minion-bl
    COMPONENT minion-bl
)

