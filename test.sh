#!/usr/bin/env bash
# Configure, build, and run tests (ctest), then optionally run pingstats with one or more hosts.
# Comments describe each step; behavior is unchanged.
# Usage:
#   ./test.sh [-c Debug|Release] [--] host1 [host2 ...]
# Examples:
#   ./test.sh                 # build+ctest only (Debug)
#   ./test.sh -c Release      # build+ctest in Release
#   ./test.sh 8.8.8.8 1.1.1.1 # build+ctest, then run pingstats on the given hosts

set -euo pipefail

CONFIG="Debug"

HOSTS=()
while [[ $# -gt 0 ]]; do
  case "$1" in
    -c|--config)
      CONFIG=${2:-}
      if [[ -z "$CONFIG" ]]; then
        echo "Error: missing value for --config" >&2
        exit 2
      fi
      shift 2
      ;;
    -h|--help)
      cat <<'EOF'
Usage: ./test.sh [-c Debug|Release] [--] host1 [host2 ...]

Configures (if needed) and builds the project in ./build, then runs ctest.
If hosts are provided, pingstats is executed afterwards with those hosts.
EOF
      exit 0
      ;;
    --)
      shift
      HOSTS+=("$@")
      break
      ;;
    -*)
      echo "Error: unknown option $1" >&2
      exit 2
      ;;
    *)
      HOSTS+=("$1")
      shift
      ;;
  esac
done

BUILD_DIR="build"
mkdir -p "$BUILD_DIR"

# Configure project into chosen build directory (generator inferred).
echo "[cmake] Configuring ($CONFIG)"
cmake -S . -B "$BUILD_DIR"

# Build requested configuration (Debug/Release).
echo "[cmake] Building ($CONFIG)"
cmake --build "$BUILD_DIR" --config "$CONFIG"

# Run tests with verbose failures for quick triage.
echo "[ctest] Running tests ($CONFIG)"
ctest --test-dir "$BUILD_DIR" --build-config "$CONFIG" --output-on-failure

if [[ ${#HOSTS[@]} -gt 0 ]]; then
  # Find built pingstats (handles .exe and non-.exe) and run with provided hosts.
  CANDIDATES=(
    "$BUILD_DIR/$CONFIG/pingstats.exe"
    "$BUILD_DIR/pingstats.exe"
    "$BUILD_DIR/$CONFIG/pingstats"
    "$BUILD_DIR/pingstats"
  )
  EXE=""
  for c in "${CANDIDATES[@]}"; do
    if [[ -x "$c" ]]; then
      EXE="$c"
      break
    fi
  done
  if [[ -z "$EXE" ]]; then
    echo "Error: pingstats executable not found in $BUILD_DIR (checked config and root)" >&2
    exit 1
  fi
  echo "[run] $EXE ${HOSTS[*]}"
  "$EXE" "${HOSTS[@]}"
fi
