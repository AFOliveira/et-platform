########################
# Layer 1 peripheral drivers (header-only)
#
# Exposes hardware-access wrappers (timer, ipi, thread, mprot, plic, uart)
# as portable inline helpers over a per-platform backend selected by
# ET_PLATFORM_ERBIUM. Ported from the earlier hal/api + hal/platform/<soc>
# tree.
#
# Installed as an INTERFACE target so external consumers (Zephyr,
# standalone bootloaders) can link with et-common-libs::drivers and get
# the right headers on the include path.
########################

set(DRIVERS_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/drivers)

set(DRIVERS_HDRS
    include/drivers/common/timer.h
    include/drivers/etsoc/timer_platform.h
    include/drivers/erbium/timer_platform.h
    include/drivers/common/ipi.h
    include/drivers/etsoc/ipi_platform.h
    include/drivers/erbium/ipi_platform.h
    include/drivers/common/thread.h
    include/drivers/etsoc/thread_platform.h
    include/drivers/erbium/thread_platform.h
    include/drivers/common/mprot.h
    include/drivers/etsoc/mprot_platform.h
    include/drivers/erbium/mprot_platform.h
    include/drivers/common/plic.h
    include/drivers/etsoc/plic_platform.h
    include/drivers/erbium/plic_platform.h
    include/drivers/common/uart.h
    include/drivers/etsoc/uart_platform.h
    include/drivers/erbium/uart_platform.h
    include/drivers/erbium/shakti_uart.h
)

add_library(drivers INTERFACE)
add_library(et-common-libs::drivers ALIAS drivers)
target_include_directories(drivers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${DRIVERS_INSTALL_PREFIX}/include>
)

MACRO(InstallHdrsWithDirStruct HEADER_LIST)
    FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/\]" DIR ${HEADER})
    INSTALL(FILES ${HEADER} DESTINATION ${DRIVERS_INSTALL_PREFIX}/${DIR})
    ENDFOREACH(HEADER)
ENDMACRO(InstallHdrsWithDirStruct)

InstallHdrsWithDirStruct(DRIVERS_HDRS)

install(
    TARGETS drivers
    EXPORT driversTargets
    INCLUDES DESTINATION ${DRIVERS_INSTALL_PREFIX}/include
)

install(
    EXPORT driversTargets
    NAMESPACE et-common-libs::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/et-common-libs/drivers
    COMPONENT drivers
)
