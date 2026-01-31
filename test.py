#!/usr/bin/env python3
"""Configure, build, and run tests with CMake/ctest, then optionally run pingstats.

Comments in this script document the flow without changing behavior.

Usage:
  python test.py [-c Debug|Release] [-b build_dir] [-- host1 [host2 ...]]

Examples:
  python test.py                       # build+ctest only (Debug)
  python test.py -c Release            # build+ctest in Release
  python test.py -- 8.8.8.8 1.1.1.1    # build+ctest, then run pingstats for the hosts
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path


def run(cmd: list[str], cwd: Path | None = None) -> None:
    """Execute a command, echo it first, and exit with the same code on failure."""
    print(f"[cmd] {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, check=False)
    if result.returncode != 0:
        raise SystemExit(result.returncode)


def main(argv: list[str]) -> int:
    """Parse args, configure/build/test with CMake/ctest, then optionally run pingstats."""
    parser = argparse.ArgumentParser(description="Configure, build, and run ctest; optionally run pingstats.")
    parser.add_argument("-c", "--config", choices=["Debug", "Release"], default="Debug", help="CMake configuration")
    parser.add_argument("-b", "--build-dir", default="build", help="Build directory")
    parser.add_argument("hosts", nargs="*", help="Hosts to run with pingstats after tests (optional)")
    args = parser.parse_args(argv)

    build_dir = Path(args.build_dir)
    build_dir.mkdir(parents=True, exist_ok=True)

    # Configure (generator inferred) into the chosen build directory.
    run(["cmake", "-S", ".", "-B", str(build_dir)])

    # Build the requested configuration (multi-config generators honor --config).
    run(["cmake", "--build", str(build_dir), "--config", args.config])

    # Run tests with output-on-failure for easier triage.
    run(["ctest", "--test-dir", str(build_dir), "--build-config", args.config, "--output-on-failure"])

    # Optional: run pingstats on provided hosts (resolve common executable names).
    if args.hosts:
        candidates = [
            build_dir / args.config / "pingstats.exe",
            build_dir / "pingstats.exe",
            build_dir / args.config / "pingstats",
            build_dir / "pingstats",
        ]
        exe = next((c for c in candidates if c.exists()), None)
        if exe is None:
            print(f"Error: pingstats executable not found in {build_dir}", file=sys.stderr)
            return 1
        run([str(exe), *args.hosts])

    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
