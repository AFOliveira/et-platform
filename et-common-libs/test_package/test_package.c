#if defined(TEST_PACKAGE_SP_BL)

// sp_bl1
#include <drivers/etsoc/pmu/pmu.h>
#include <isa/common/io.h>
#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/etsoc_memory.h>
#include <isa/etsoc/esr_defines.h>
#include <isa/common/macros.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/flb.h>
#include <isa/common/hart.h>
#include <isa/etsoc/sync.h>
#include <isa/etsoc/cacheops.h>
#include <isa/common/riscv_encoding.h>
#include <isa/etsoc/utils.h>
#include <system/layout.h>
#include <system/etsoc_ddr_region_map.h>

#include <drivers/etsoc/serial/serial.h>

// sp_bl2
#include <common/etsoc/common_defs.h>
#include <common/etsoc/log_common.h>
#include <drivers/etsoc/pcie/pcie_int.h>
#include <drivers/etsoc/pmu/pmu.h>
#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/etsoc_memory.h>
#include <isa/common/io.h>
#include <isa/etsoc/cacheops.h>
#include <isa/etsoc/esr_defines.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/utils.h>
//#include <transports/vq/vq.h>  // commented out due 'etsoc_rt_memory.h:53:2: error: #error "Definition for device runtime memory access not provided!"'
#include <transports/circbuff/circbuff.h>
#include <transports/sp_mm_iface/sp_mm_comms_spec.h>
#include <transports/mm_cm_iface/message_types.h>
//#include <transports/sp_mm_iface/sp_mm_iface.h> // commented out due 'etsoc_rt_memory.h:53:2: error: #error "Definition for device runtime memory access not provided!"'
#include <transports/sp_mm_iface/sp_mm_shared_config.h>
#include <system/layout.h>
#include <system/etsoc_ddr_region_map.h>

#include <drivers/etsoc/serial/serial.h>
//#include <isa/etsoc/etsoc_rt_memory.h>

#elif defined(TEST_PACKAGE_CM_UMODE)

#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/barriers.h>
//#include <isa/etsoc/cacheops.h> clases with cacheops-umode.h
#include <isa/etsoc/cacheops-umode.h>
#include <isa/etsoc/esr_defines.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/flb.h>
#include <isa/common/hart.h>
#include <isa/common/syscall.h>
#include <isa/common/tensors.h>
#include <isa/etsoc/utils.h>
#include <trace/trace_umode.h>
#include <trace/trace_umode_cb.h>

#include <common/etsoc/utils.h>
#include <drivers/etsoc/pmu/pmu.h>

#elif defined(TEST_PACKAGE_MINION_BL)

#include <drivers/etsoc/pmu/pmu.h>
#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/etsoc_memory.h>
#include <isa/etsoc/esr_defines.h>
#include <isa/common/macros.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/flb.h>
#include <isa/common/hart.h>
#include <isa/common/io.h>
#include <isa/etsoc/sync.h>
#include <isa/etsoc/cacheops.h>
#include <isa/common/syscall.h>
#include <isa/common/riscv_encoding.h>
#include <isa/etsoc/utils.h>
#include <system/etsoc_ddr_region_map.h>
#include <system/layout.h>

#include <transports/mm_cm_iface/broadcast.h>

#elif defined(TEST_PACKAGE_MM_RT_SVCS)

#include <common/etsoc/common_defs.h>
#include <common/etsoc/log_common.h>
#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/cacheops.h>
#include <isa/etsoc/esr_defines.h>
#include <isa/etsoc/etsoc_memory.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/flb.h>
#include <isa/common/hart.h>
#include <isa/common/io.h>
#include <isa/common/macros.h>
#include <isa/etsoc/sync.h>
#include <isa/common/syscall.h>
#include <isa/common/riscv_encoding.h>
#include <isa/etsoc/utils.h>
#include <system/etsoc_ddr_region_map.h>
#include <system/layout.h>

#include <common/printf.h>
#include <drivers/etsoc/serial/serial.h>
#include <drivers/etsoc/pcie/pcie_int.h>
#include <drivers/etsoc/pcie/pcie_device.h>
#include <drivers/etsoc/pmu/pmu.h>
#include <isa/etsoc/etsoc_rt_memory.h>
#include <transports/circbuff/circbuff.h>
#include <transports/vq/vq.h>
#include <transports/mm_cm_iface/broadcast.h>
#include <transports/mm_cm_iface/message_types.h>
#include <transports/sp_mm_iface/sp_mm_comms_spec.h>
#include <transports/sp_mm_iface/sp_mm_iface.h>
#include <transports/sp_mm_iface/sp_mm_shared_config.h>

#elif defined(TEST_PACKAGE_CM_RT_SVCS)

#include <common/etsoc/common_defs.h>
#include <common/etsoc/log_common.h>
#include <isa/common/atomic.h>
//#include <isa/common/atomic-impl.h> clashes with atomic.h
#include <isa/etsoc/esr_defines.h>
#include <isa/etsoc/fcc.h>
#include <isa/etsoc/flb.h>
#include <isa/common/hart.h>
#include <isa/common/io.h>
#include <isa/etsoc/etsoc_memory.h>
#include <isa/common/macros.h>
#include <isa/etsoc/cacheops.h>
#include <isa/etsoc/sync.h>
#include <isa/common/syscall.h>
#include <isa/common/riscv_encoding.h>
#include <isa/etsoc/utils.h>
#include <system/etsoc_ddr_region_map.h>
#include <system/layout.h>

#include <common/printf.h>
#include <transports/circbuff/circbuff.h>
#include <drivers/etsoc/pmu/pmu.h>
#include <drivers/etsoc/serial/serial.h>
#include <transports/mm_cm_iface/message_types.h>

#else
#error "we should not reach here.."
#endif

int main()
{
    return 0;
}