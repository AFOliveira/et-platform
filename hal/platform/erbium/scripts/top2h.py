#!/usr/bin/env python3
"""Generate a simple top-level base-address header from a top RDL file.

The input is a top-level memory-map style SystemRDL file such as
`top_cpu_mm.rdl`, where the important information is the placement of block
instances:

    System_Reg system_registers @0x2000000;
    UART_Reg   uart_registers   @0x2004000;
    Erbium_ESR cpu_registers    @0x80000000;

This script intentionally does not elaborate the full SystemRDL model. The top
maps often reference optional submodule RDLs that may be absent in a partial
checkout. Instead, it text-parses the top file and any available `include`d
RDLs to recover the address placements we need.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import re
import sys
from pathlib import Path


ADDRMAP_RE = re.compile(r"^\s*addrmap\s+([A-Za-z_][A-Za-z0-9_]*)\s*\{")
ADDR_RE = re.compile(r"@\s*(0x[0-9A-Fa-f_]+|[0-9_]+)")
INCLUDE_RE = re.compile(r'^\s*`include\s+"([^"]+)"')


@dataclass(frozen=True)
class Placement:
    kind: str
    instance: str
    export_name: str
    address: int


def hex_literal(value: int) -> str:
    if value <= 0xFFFFFFFF:
        return f"0x{value:08X}ul"
    return f"0x{value:016X}ull"


def sanitize_lines(path: Path) -> list[str]:
    return [
        raw_line.split("//", 1)[0].strip()
        for raw_line in path.read_text(encoding="utf-8").splitlines()
    ]


def parse_available_includes(root: Path) -> list[Path]:
    seen: set[Path] = set()
    ordered: list[Path] = []

    def walk(path: Path) -> None:
        path = path.resolve()
        if path in seen or not path.exists():
            return
        seen.add(path)
        ordered.append(path)
        for line in sanitize_lines(path):
            m = INCLUDE_RE.match(line)
            if not m:
                continue
            inc = (path.parent / m.group(1)).resolve()
            if inc.exists():
                walk(inc)

    walk(root)
    return ordered


def parse_placement_line(line: str) -> Placement | None:
    if "@" not in line or ";" not in line:
        return None

    left, _ = line.split("@", 1)
    tokens = left.strip().split()
    if not tokens or tokens[0] == "property":
        return None

    addr_match = ADDR_RE.search(line)
    if not addr_match:
        return None

    instance = tokens[-1]
    kind = " ".join(tokens[:-1])
    export_name = tokens[1] if tokens[0] == "external" and len(tokens) >= 3 else instance
    address = int(addr_match.group(1).replace("_", ""), 0)
    return Placement(kind=kind, instance=instance, export_name=export_name, address=address)


def parse_addrmaps(path: Path) -> dict[str, list[Placement]]:
    addrmaps: dict[str, list[Placement]] = {}
    top_name = None
    entries: list[Placement] = []
    brace_depth = 0
    in_top = False

    for line in sanitize_lines(path):
        if not line:
            continue

        if not in_top:
            m = ADDRMAP_RE.match(line)
            if m:
                top_name = m.group(1)
                in_top = True
                brace_depth = line.count("{") - line.count("}")
            continue

        brace_depth += line.count("{") - line.count("}")

        if brace_depth <= 0:
            break

        placement = parse_placement_line(line)
        if placement is not None:
            entries.append(placement)

    if top_name is not None:
        addrmaps[top_name] = entries
    return addrmaps


def parse_top_rdl(path: Path):
    all_addrmaps: dict[str, list[Placement]] = {}
    for file_path in parse_available_includes(path):
        all_addrmaps.update(parse_addrmaps(file_path))

    top_defs = parse_addrmaps(path)
    if not top_defs:
        sys.exit(f"[top2h] no addrmap found in {path}")
    top_name = next(iter(top_defs))
    entries = all_addrmaps.get(top_name, [])
    if not entries:
        sys.exit(f"[top2h] no top-level placements found in {path}")
    return top_name, entries, all_addrmaps


def emit_placement(out: list[str], macro_prefix: str, placement: Placement,
                   absolute_address: int, seen_macros: dict[str, int]) -> None:
    macro = f"{macro_prefix}_{placement.export_name.upper()}_BASE"
    if macro in seen_macros:
        if seen_macros[macro] != absolute_address:
            sys.exit(
                f"[top2h] conflicting values for {macro}: "
                f"0x{seen_macros[macro]:X} vs 0x{absolute_address:X}"
            )
        return

    seen_macros[macro] = absolute_address
    out.append(f"/* Instance: {placement.kind} {placement.instance} */")
    out.append(f"#define {macro} {hex_literal(absolute_address)}")
    out.append("")


def emit_nested_placements(out: list[str], macro_prefix: str,
                           placement: Placement, absolute_address: int,
                           addrmaps: dict[str, list[Placement]],
                           seen_macros: dict[str, int]) -> None:
    for child in addrmaps.get(placement.kind, []):
        if not child.kind.startswith("external "):
            continue
        child_abs = absolute_address + child.address
        emit_placement(out, macro_prefix, child, child_abs, seen_macros)


def emit_header(input_path: Path, output_path: Path, prefix: str | None) -> str:
    top_name, entries, addrmaps = parse_top_rdl(input_path)
    macro_prefix = prefix or top_name.upper()
    guard = f"_{output_path.name.replace('.', '_').upper()}_"
    seen_macros: dict[str, int] = {}

    out = []
    out.append(f"/* Auto-generated by top2h.py from {input_path.name}. Do not edit. */")
    out.append(f"#ifndef {guard}")
    out.append(f"#define {guard}")
    out.append("")
    out.append("/* ============================================================ */")
    out.append(f"/* Top map: {top_name} */")
    out.append("/* ============================================================ */")
    out.append("")

    for placement in entries:
        emit_placement(out, macro_prefix, placement, placement.address, seen_macros)
        emit_nested_placements(
            out, macro_prefix, placement, placement.address, addrmaps, seen_macros
        )

    out.append(f"#endif /* {guard} */")
    out.append("")
    return "\n".join(out)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("input", type=Path, help="Top-level RDL file to parse.")
    ap.add_argument("-o", "--out", type=Path, required=True,
                    help="Output header path.")
    ap.add_argument("-p", "--prefix", default=None,
                    help="Macro prefix override (default: top addrmap name).")
    args = ap.parse_args()

    text = emit_header(args.input.resolve(), args.out.resolve(), args.prefix)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(text, encoding="utf-8")


if __name__ == "__main__":
    main()
