##########################
# Minion Run Time Services
##########################

#Minion Boot Loader requires etsoc_hal library
find_package(etsoc_hal REQUIRED)

#Install prefix for Minion Bootloader library
set(MM_RT_SVCS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/mm-rt-svcs)

################################################
# List the public interfaces and headers to be
# exposed to Master Minion Runtime Sevrices
################################################

#Listing of header only public interfaces
set(MM_RT_SVCS_HDRS
    include/common/etsoc/common_defs.h
    include/common/etsoc/log_common.h
    include/isa/common/atomic.h
    include/isa/common/atomic-impl.h
    include/isa/etsoc/cacheops.h
    include/isa/common/cacheops_common.h
    include/isa/etsoc/esr_defines.h
    include/isa/etsoc/etsoc_memory.h
    include/isa/etsoc/fcc.h
    include/isa/etsoc/flb.h
    include/isa/common/hart.h
    include/isa/common/io.h
    include/isa/common/macros.h
    include/isa/etsoc/sync.h
    include/isa/common/syscall.h
    include/isa/common/riscv_encoding.h
    include/isa/etsoc/utils.h
    include/system/abi.h
    include/system/etsoc_ddr_region_map.h
    include/system/layout.h
)

#Listing of public headers that expose services provided by
#the MINION_BL (Minion Bootloader Library
set(MM_RT_SVCS_LIB_HDRS
    include/common/printf.h
    include/drivers/etsoc/serial/serial.h
    include/drivers/etsoc/pcie/pcie_int.h
    include/drivers/etsoc/pcie/pcie_device.h
    include/drivers/etsoc/pmu/pmu.h
    include/isa/etsoc/etsoc_rt_memory.h
    include/transports/circbuff/circbuff.h
    include/transports/vq/vq.h
    include/transports/mm_cm_iface/broadcast.h
    include/transports/mm_cm_iface/message_types.h
    include/transports/sp_mm_iface/sp_mm_comms_spec.h
    include/transports/sp_mm_iface/sp_mm_iface.h
    include/transports/sp_mm_iface/sp_mm_shared_config.h
)

##########################
#Create mm-rt-svcs library
##########################

#Listing of sources that implement services provided by
#the MM_RT_SVCS (Master Minion Runtime Services) Library
add_library(mm-rt-svcs STATIC
    src/common/printf.c
    src/drivers/etsoc/serial/serial.c
    src/drivers/etsoc/pcie/pcie_int.c
    src/drivers/etsoc/pmu/pmu.c
    src/isa/etsoc/etsoc_memory.c
    src/transports/circbuff/circbuff.c
    src/transports/vq/vq.c
    src/transports/mm_cm_iface/broadcast.c
    src/transports/sp_mm_iface/sp_mm_iface.c
)

set_target_properties(mm-rt-svcs PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(mm-rt-svcs
    PUBLIC
        etsoc_hal::etsoc_hal
        esperantoTrace::et_trace
)

target_include_directories(mm-rt-svcs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${MM_RT_SVCS_INSTALL_PREFIX}/include>
)

target_compile_features(mm-rt-svcs PUBLIC c_std_11)

target_compile_options(mm-rt-svcs
    PRIVATE
        -Wall
        -fno-strict-aliasing
        $<$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>:-Werror>
)

target_compile_definitions(mm-rt-svcs
    PUBLIC
        -DMM_RT=1
)

###################################################
#Install and export mmm-rt-svcs library and headers
###################################################

#This macro preserves the driectory structure as defined by the
#MM_RT_SVCS listing above
MACRO(InstallHdrsWithDirStruct HEADER_LIST)
    FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/\]" DIR ${HEADER})
    INSTALL(FILES ${HEADER} DESTINATION ${MM_RT_SVCS_INSTALL_PREFIX}/${DIR})
    ENDFOREACH(HEADER)
ENDMACRO(InstallHdrsWithDirStruct)

InstallHdrsWithDirStruct(MM_RT_SVCS_HDRS)
InstallHdrsWithDirStruct(MM_RT_SVCS_LIB_HDRS)

install(
    TARGETS mm-rt-svcs
    EXPORT mm-rt-svcsTargets
    LIBRARY DESTINATION ${MM_RT_SVCS_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${MM_RT_SVCS_INSTALL_PREFIX}/lib
    INCLUDES DESTINATION ${MM_RT_SVCS_INSTALL_PREFIX}/include
)

#TODO: Could be improved and made more flexible by exporting a package
install(
    EXPORT mm-rt-svcsTargets
    NAMESPACE et-common-libs::
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/et-common-libs/mm-rt-svcs
    COMPONENT mm-rt-svcs
)

