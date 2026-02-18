# GDB script for thread_disable_consistency test
# Tests that GDB resume does not re-enable unavailable harts

target remote localhost:1337

# Set breakpoints
break thread_disable_consistency.c:51
break thread_disable_consistency.c:69

# Run through breakpoints to completion
continue
continue
continue
