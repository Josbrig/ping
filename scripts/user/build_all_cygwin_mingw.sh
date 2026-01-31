#!/usr/bin/env bash
# User script: configure, build, test, and generate docs for Cygwin/MinGW (single-config generators).
# Builds Debug, Release, and RelWithDebInfo in separate build dirs; runs ctest; tries Doxygen.

set -euo pipefail

CONFIGS=(Debug Release RelWithDebInfo)
BUILD_PREFIX="build"
GENERATOR=""

usage() {
  cat <<'EOF'
Usage: ./scripts/user/build_all_cygwin_mingw.sh [-g GENERATOR]

Options:
  -g, --generator GENERATOR   Optional CMake generator (e.g., "Ninja", "Unix Makefiles").

Behavior:
  - Configures, builds, and tests each of Debug/Release/RelWithDebInfo.
  - Uses separate build directories: build-debug, build-release, build-relwithdebinfo.
  - Runs ctest with output-on-failure.
  - Tries to build 'doxygen' once (Release); failure is non-fatal.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -g|--generator)
      GENERATOR=${2:-}
      if [[ -z "$GENERATOR" ]]; then
        echo "Error: missing generator value" >&2
        exit 2
      fi
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Error: unknown option $1" >&2
      exit 2
      ;;
  esac
done

run_cmake() {
  echo "[cmake] $*"
  cmake "$@"
}

run_ctest() {
  echo "[ctest] $*"
  ctest "$@"
}

for cfg in "${CONFIGS[@]}"; do
  lower=$(echo "$cfg" | tr '[:upper:]' '[:lower:]')
  build_dir="${BUILD_PREFIX}-${lower}"

  args=(-S . -B "$build_dir" -DCMAKE_BUILD_TYPE="$cfg")
  if [[ -n "$GENERATOR" ]]; then
    args+=(-G "$GENERATOR")
  fi
  run_cmake "${args[@]}"

  run_cmake --build "$build_dir"

  run_ctest --test-dir "$build_dir" --output-on-failure
done

# Attempt docs once (Release build) if target exists; ignore failure.
DOC_DIR="${BUILD_PREFIX}-release"
if [[ -d "$DOC_DIR" ]]; then
  if cmake --build "$DOC_DIR" --target doxygen >/dev/null 2>&1; then
    echo "[docs] doxygen built in $DOC_DIR"
  else
    echo "[docs] doxygen target unavailable or failed; skipping" >&2
  fi
else
  echo "[docs] Release build directory not found; skipping docs" >&2
fi
