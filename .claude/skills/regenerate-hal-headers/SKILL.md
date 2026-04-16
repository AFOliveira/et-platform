---
name: regenerate-hal-headers
description: Regenerate hal/platform/erbium/hwinc/ register headers from Erbium SystemRDL sources. Use when the user asks to regenerate HAL headers, update register defs, or pick up RDL changes.
---

# Regenerate Erbium HAL headers

`hal/platform/erbium/scripts/gen_all_headers.py` rebuilds every
header under `hal/platform/erbium/hwinc/` from the Erbium RDL sources.
The script has no hardcoded RTL-tree paths — every input must be
provided via CLI flag or env var.

## RDL tree location

On this machine the hardware RDL tree is checked out at:

```
/home/afonso/docsET/hdl-et
```

## Run the full pipeline

```bash
cd /home/afonso/et-platform

export ERBIUM_HAL_UART_RDL=/home/afonso/docsET/hdl-et/regblocks/systemrdl/uart.rdl
export ERBIUM_HAL_SYSTEM_RDL=/home/afonso/docsET/hdl-et/regblocks/systemrdl/system.rdl
export ERBIUM_HAL_QSPI_RDL=/home/afonso/docsET/hdl-et/regblocks/systemrdl/qspi.rdl
export ERBIUM_HAL_I2C_RDL=/home/afonso/docsET/hdl-et/ip/i2c/hdl/systemrdl/i2c.rdl
export ERBIUM_HAL_ESR_RDL=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/esr/scripts/systemRDL/esr.rdl
export ERBIUM_HAL_PLIC_RDL=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/esr/scripts/systemRDL/plic.rdl
export ERBIUM_HAL_MINION_CSR_RDL=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/generated/minion_csr.rdl
export ERBIUM_HAL_TOP_RDL=/home/afonso/docsET/hdl-et/regblocks/systemrdl/top_cpu_mm.rdl

# Inputs for csr2rdl.py (CSV -> minion_csr.rdl)
export ERBIUM_HAL_CSR_SEMIFORE_CSV=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/semifore/minion_csr.csv
export ERBIUM_HAL_CSR_REG_CSV=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/scripts/csr.csv
export ERBIUM_HAL_CSR_TYPES_DIR=/home/afonso/docsET/hdl-et/ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/scripts/types

python3 hal/platform/erbium/scripts/gen_all_headers.py
```

This rewrites the 8 headers under `hal/platform/erbium/hwinc/`:
`esr.h, i2c.h, minion_csr.h, plic.h, qspi.h, system.h, top.h, uart.h`.

## Skip the CSV → RDL step

If the CSV sources haven't changed, skip `csr2rdl.py` and use the
existing `minion_csr.rdl`:

```bash
python3 hal/platform/erbium/scripts/gen_all_headers.py --skip-csr-regen
```

When `--skip-csr-regen` is set, `ERBIUM_HAL_CSR_SEMIFORE_CSV`,
`ERBIUM_HAL_CSR_REG_CSV`, and `ERBIUM_HAL_CSR_TYPES_DIR` are not
required.

## Write to a different directory

```bash
python3 hal/platform/erbium/scripts/gen_all_headers.py --out /tmp/hwinc
```

Useful for diff-checking a regeneration against the committed headers:

```bash
diff -qr /tmp/hwinc hal/platform/erbium/hwinc/
```

## Scripts invoked

- `hal/platform/erbium/scripts/gen_all_headers.py` — driver
- `hal/platform/erbium/scripts/csr2rdl.py` — CSV → RDL (Minion CSRs)
- `hal/platform/erbium/scripts/rdl2hal.py` — RDL → C header (per peripheral)
- `hal/platform/erbium/scripts/top2h.py` — top map RDL → top.h

All three helpers are shipped in this repo. `csr2rdl.py` was
originally authored by the hardware team and adapted here to remove
RTL-layout-specific defaults.

## Verifying a regeneration

After running, the committed `hwinc/` should match the freshly
generated output exactly if the RDL sources haven't changed:

```bash
python3 hal/platform/erbium/scripts/gen_all_headers.py --out /tmp/hwinc-regen
diff -qr /tmp/hwinc-regen hal/platform/erbium/hwinc/
# expect no output
```

If there's a diff, inspect which register changed and commit the
updated `hwinc/` alongside any dependent code changes.
