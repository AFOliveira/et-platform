#!/bin/bash
#
# Verify preload address validation for Erbium (PA_SIZE=32).
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
BUILD_DIR="$REPO_ROOT/sw-sysemu/build"
EMU="$BUILD_DIR/erbium_emu"

RISCV="${RISCV:-/home/afonso/et/bin}"
GCC="$RISCV/riscv64-unknown-elf-gcc"
TEST_ELF="$SCRIPT_DIR/build/thread_disable_consistency.elf"
RUN_TIMEOUT_SEC=20

TMPDIR="$(mktemp -d)"
LOG_FILE="$TMPDIR/loader_validation.log"
trap 'pkill -9 erbium_emu 2>/dev/null || true; rm -rf "$TMPDIR"' EXIT

run_case() {
    local description="$1"
    shift

    : > "$LOG_FILE"
    set +e
    timeout "${RUN_TIMEOUT_SEC}s" "$EMU" "$@" > "$LOG_FILE" 2>&1
    local rc=$?
    set -e

    if [ "$rc" -eq 124 ]; then
        echo "FAIL: $description (timeout)"
        tail -40 "$LOG_FILE"
        return 1
    fi

    if ! grep -q "exceeds 32-bit physical address space" "$LOG_FILE"; then
        echo "FAIL: $description (missing validation message)"
        tail -40 "$LOG_FILE"
        return 1
    fi

    echo "PASS: $description"
}

if [ ! -x "$GCC" ]; then
    echo "ERROR: RISC-V GCC not found at $GCC"
    exit 1
fi

echo "Building erbium_emu..."
make -C "$BUILD_DIR" erbium_emu -j"$(nproc)" > /dev/null

echo "Building Erbium test ELFs..."
make -C "$SCRIPT_DIR" build RISCV="$RISCV" > /dev/null

cat > "$TMPDIR/high_addr.S" <<'EOF'
.section .text
.globl _start
_start:
  j _start
EOF

"$GCC" -nostdlib -march=rv64imfc -mabi=lp64f \
    -Wl,-Ttext=0x100001000 \
    -o "$TMPDIR/high_addr.elf" \
    "$TMPDIR/high_addr.S"

printf '\xAA\xBB\xCC\xDD' > "$TMPDIR/payload.bin"

run_case "ELF section above PA32 is rejected" \
    -elf_load "$TMPDIR/high_addr.elf" -l -max_cycles 1

run_case "file_load above PA32 is rejected" \
    -elf_load "$TEST_ELF" \
    -file_load "0x100000000,$TMPDIR/payload.bin" \
    -l -max_cycles 1

run_case "mem_write32 above PA32 is rejected" \
    -elf_load "$TEST_ELF" \
    -mem_write32 "0x100000000,0x1" \
    -l -max_cycles 1

echo "SUCCESS: loader address validation checks passed"
