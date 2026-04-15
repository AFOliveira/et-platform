#!/usr/bin/env python3
"""Regenerate every header under hal/erbium-hal/include/hwinc/ from RDL sources.

Pipeline:

    CSV (+ types/*.csv)  ─► csr2rdl.py  ─► minion_csr.rdl  ─┐
                                                            │
                                                            ├─► rdl2hal.py ─► hwinc/*.h
    hand-written RDLs   ────────────────────────────────────┘
    (uart, system, qspi, i2c, plic, esr)

All paths are supplied by the caller (CLI flags or env vars). Nothing
about this script assumes a particular location for the RDL tree, so
it works no matter where the hdl repo is checked out.

The minion_csr CSV->RDL step is optional. Use --skip-csr-regen when
you already have a committed minion_csr.rdl you want to keep using.

Usage:
    gen_all_headers.py --rdl-root /path/to/hdl-et
    gen_all_headers.py --rdl-root ~/docsET/hdl-et --skip-csr-regen
    gen_all_headers.py --rdl-root ~/docsET/hdl-et --out /tmp/hwinc

    # all paths can come from env vars instead of flags:
    ERBIUM_HAL_RDL_ROOT=~/docsET/hdl-et gen_all_headers.py

The list of peripheral RDLs is the SOURCES table below; edit there
when a new peripheral is added.
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path


# Directory containing this script (hal/erbium-hal/scripts/).
SCRIPT_DIR = Path(__file__).resolve().parent

# Default output = the sibling hwinc/ directory under this HAL.
DEFAULT_OUT = (SCRIPT_DIR.parent / "include" / "hwinc").resolve()

# rdl2hal.py lives next to this script.
RDL2HAL = SCRIPT_DIR / "rdl2hal.py"


# (rdl_path_relative_to_rdl_root, output_header_name, optional_top_addrmap)
SOURCES = [
    ("regblocks/systemrdl/uart.rdl",
        "uart.h",        None),
    ("regblocks/systemrdl/system.rdl",
        "system.h",      None),
    ("regblocks/systemrdl/qspi.rdl",
        "qspi.h",        None),
    ("ip/i2c/hdl/systemrdl/i2c.rdl",
        "i2c.h",         "I2C_Reg"),
    ("ip/cpu_subsystem/rtl/shire/esr/scripts/systemRDL/esr.rdl",
        "esr.h",         "Erbium_ESR"),
    ("ip/cpu_subsystem/rtl/shire/esr/scripts/systemRDL/plic.rdl",
        "plic.h",        "PLIC_cpu"),
    ("ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/generated/minion_csr.rdl",
        "minion_csr.h",  None),
]

# Where csr2rdl.py lives inside the RDL tree (relative to --rdl-root).
CSR2RDL_REL     = "ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/scripts/csr2rdl.py"
# Where the RDL output of csr2rdl.py ends up.
MINION_CSR_RDL  = "ip/cpu_subsystem/rtl/shire/minion/intpipe/csr/generated/minion_csr.rdl"


def run(cmd: list[str]) -> None:
    printable = " ".join(str(c) for c in cmd)
    print(f"  $ {printable}")
    result = subprocess.run(cmd, check=False)
    if result.returncode != 0:
        sys.exit(f"[gen_all_headers] command failed: {printable}")


def regen_minion_csr_rdl(rdl_root: Path) -> None:
    script = rdl_root / CSR2RDL_REL
    if not script.exists():
        sys.exit(
            f"[gen_all_headers] csr2rdl.py not found at {script}\n"
            f"  (did you pass the right --rdl-root?)"
        )
    print("[1/2] CSV → RDL (minion_csr)")
    run(["python3", str(script), "--rdl-only"])


def regen_headers(rdl_root: Path, out_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    print(f"[2/2] RDL → C headers  (→ {out_dir})")
    for rel_rdl, out_name, top in SOURCES:
        rdl = (rdl_root / rel_rdl).resolve()
        if not rdl.exists():
            sys.exit(f"[gen_all_headers] missing RDL source: {rdl}")
        cmd = [
            "python3", str(RDL2HAL),
            str(rdl),
            "-o", str(out_dir / out_name),
        ]
        if top:
            cmd += ["-t", top]
        run(cmd)


def resolve_rdl_root(cli_value: Path | None) -> Path:
    if cli_value is not None:
        return cli_value.expanduser().resolve()
    env = os.environ.get("ERBIUM_HAL_RDL_ROOT")
    if env:
        return Path(env).expanduser().resolve()
    sys.exit(
        "[gen_all_headers] --rdl-root not provided and "
        "ERBIUM_HAL_RDL_ROOT env var not set.\n"
        "  Usage: gen_all_headers.py --rdl-root /path/to/hdl-et"
    )


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument(
        "--rdl-root", type=Path, default=None,
        help="Root of the hdl-et repo containing the RDL sources. "
             "Defaults to $ERBIUM_HAL_RDL_ROOT.",
    )
    ap.add_argument(
        "--out", type=Path, default=DEFAULT_OUT,
        help=f"Output directory for generated headers "
             f"(default: {DEFAULT_OUT}).",
    )
    ap.add_argument(
        "--skip-csr-regen", action="store_true",
        help="Skip the CSV→RDL step and use the committed minion_csr.rdl.",
    )
    args = ap.parse_args()

    rdl_root = resolve_rdl_root(args.rdl_root)
    out_dir = args.out.expanduser().resolve()

    if not rdl_root.is_dir():
        sys.exit(f"[gen_all_headers] --rdl-root is not a directory: {rdl_root}")

    if not RDL2HAL.exists():
        sys.exit(f"[gen_all_headers] rdl2hal.py not found at {RDL2HAL}")

    if not args.skip_csr_regen:
        regen_minion_csr_rdl(rdl_root)
    else:
        print("[skip] CSV → RDL (using committed minion_csr.rdl)")
        minion_rdl = rdl_root / MINION_CSR_RDL
        if not minion_rdl.exists():
            sys.exit(
                f"[gen_all_headers] {minion_rdl} does not exist — "
                f"cannot --skip-csr-regen"
            )

    regen_headers(rdl_root, out_dir)
    print(f"\nWrote {len(SOURCES)} headers to {out_dir}")


if __name__ == "__main__":
    main()
