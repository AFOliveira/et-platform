#################################
# Service Processor Boot Loader 1
#################################

#Minion Boot Loader requires etsoc_hal library
find_package(etsoc_hal REQUIRED)

#Install prefix for Minion Bootloader 1 library
set(SP_BL1_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/sp-bl1)

################################################
# List the public interfaces and headers to be
# exposed to SP Boot Loader 1
################################################
#Listing of header only public interfaces
set(SP_BL1_HDRS
    include/drivers/etsoc/pmu/pmu.h
    include/isa/common/io.h
    include/isa/common/atomic.h
    include/isa/common/atomic-impl.h
    include/isa/etsoc/etsoc_memory.h
    include/isa/etsoc/esr_defines.h
    include/isa/common/macros.h
    include/isa/etsoc/fcc.h
    include/isa/etsoc/flb.h
    include/isa/common/hart.h
    include/isa/etsoc/sync.h
    include/isa/common/sync.h
    include/isa/etsoc/cacheops.h
    include/isa/common/cacheops.h
    include/isa/common/cacheops_common.h
    include/isa/common/riscv_encoding.h
    include/isa/etsoc/utils.h
    include/isa/common/utils.h
    include/system/layout.h
    include/system/etsoc_ddr_region_map.h
)

#Listing of public headers that expose services provided by
#the SP_BL1 (Minion Bootloader Library
set(SP_BL1_LIB_HDRS
    include/drivers/etsoc/serial/serial.h
)

#########################
#Create sp-bl1 library
#########################

#Listing of sources that implement services provided by
#the SP_BL1 (Minion Bootloader) Library
add_library(sp-bl1 STATIC
    src/drivers/etsoc/serial/serial.c
)

set_target_properties(sp-bl1 PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(sp-bl1 PUBLIC etsoc_hal::etsoc_hal)

target_include_directories(sp-bl1
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${SP_BL1_INSTALL_PREFIX}/include>
)

#################################################
#Install and export sp-bl1 library and headers
#################################################

#This macro preserves the driectory structure as defined by the
#SP_BL1 listing above
macro(InstallHdrsWithDirStruct HEADER_LIST)
    foreach(HEADER ${${HEADER_LIST}})
    string(REGEX MATCH "(.*)[/\]" DIR ${HEADER})
    install(FILES ${HEADER} DESTINATION ${SP_BL1_INSTALL_PREFIX}/${DIR})
    endforeach(HEADER)
endmacro(InstallHdrsWithDirStruct)

InstallHdrsWithDirStruct(SP_BL1_HDRS)
InstallHdrsWithDirStruct(SP_BL1_LIB_HDRS)

install(
    TARGETS sp-bl1
    EXPORT sp-bl1Targets
    LIBRARY DESTINATION ${SP_BL1_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${SP_BL1_INSTALL_PREFIX}/lib
    INCLUDES DESTINATION ${SP_BL1_INSTALL_PREFIX}/include
)

#TODO: Could be improved and made more flexible by exporting a package
install(
    EXPORT sp-bl1Targets
    NAMESPACE et-common-libs::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/et-common-libs/sp-bl1
)
