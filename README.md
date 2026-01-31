# Project Overview

pingstats is a cross-platform C++20/CMake tool that collects long-term ping statistics (latency and loss) for one or more targets. It is designed for reliable, continuous monitoring with meaningful metrics and optional exports/visualizations. Platform backends cover Linux, macOS, Windows (MSVC), WSL, Cygwin, and MinGW.

# Key Features

- Periodic ICMP echo to multiple targets with configurable interval.
- Per-target metrics: min, max, mean, median, packet loss, histogram buckets, time-series buffer.
- Console output with tables and simple time-series/histogram views.
- CSV and JSON exporters for downstream analysis.
- Cross-platform backend abstraction (factory-selected per host OS).
- Unit and integration tests via CTest.

# System Requirements

## General
- C++20 compiler toolchain and CMake ≥ 3.20.
- ICMP/RAW socket permissions as required by the host OS (root/`CAP_NET_RAW` on Linux/WSL; Administrator on Windows variants).
- Optional: Doxygen to build the `doxygen` target.

## Linux
- Grant ICMP privileges (root or `CAP_NET_RAW`) for the process; otherwise packets will be blocked.
- Standard single-config generators (`Ninja`, `Unix Makefiles`) are supported.

## macOS
- ICMP echo requires elevated privileges (`sudo`) or appropriate entitlements; otherwise probes fail.
- Uses single-config generators; no additional dependencies beyond the toolchain.

## Windows (MSVC)
- Multi-config generators (Visual Studio/MSBuild); Administrator privileges may be required for ICMP.
- Ensure outbound ICMP is allowed by Windows Defender Firewall or other policies.

## Windows (WSL)
- Build inside WSL with the Linux toolchain; ICMP requires `CAP_NET_RAW` or `sudo` within the distro.
- Windows firewall must allow outbound ICMP from WSL.

## Windows (Cygwin)
- Single-config generators; run shell as Administrator to allow ICMP.
- Same CMake/C++20 requirements as Linux.

## Windows (MinGW)
- Single-config generators; requires a MinGW C++20 toolchain and CMake ≥ 3.20.
- ICMP may require Administrator privileges depending on the environment and firewall policy.

# Build & Test

## Multi-config (Windows/MSVC)
```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build --build-config Debug --output-on-failure

cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure

cmake --build build --config RelWithDebInfo
ctest --test-dir build --build-config RelWithDebInfo --output-on-failure

# Docs (optional, if doxygen found)
cmake --build build --config Release --target doxygen
```

## Single-config (Linux/macOS/WSL/Cygwin/MinGW)
```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
ctest --test-dir build-debug --output-on-failure

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
ctest --test-dir build-release --output-on-failure

cmake -S . -B build-relwithdebinfo -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build-relwithdebinfo
ctest --test-dir build-relwithdebinfo --output-on-failure

# Docs (optional, if doxygen found)
cmake --build build-release --target doxygen
```

Convenience user scripts (see `scripts/user/`):
- Windows PowerShell: [`scripts/user/build_all_windows.ps1`](scripts/user/build_all_windows.ps1:1)
- Windows Batch: [`scripts/user/build_all_msvc.bat`](scripts/user/build_all_msvc.bat:1)
- Unix (Linux/WSL/macOS): [`scripts/user/build_all_unix.sh`](scripts/user/build_all_unix.sh:1)
- Cygwin/MinGW: [`scripts/user/build_all_cygwin_mingw.sh`](scripts/user/build_all_cygwin_mingw.sh:1)

# Usage (binary name: `pingstats`)

```bash
./build/pingstats [options] <target1> [target2 ...]
```

Examples:
- Single target (default interval): `./build/pingstats 8.8.8.8`
- Multiple targets: `./build/pingstats 8.8.8.8 1.1.1.1 example.org`
- Custom interval and CSV export: `./build/pingstats -i 1 --output-format=csv --output-file=pingstats.csv 8.8.8.8 1.1.1.1`
- JSON export: `./build/pingstats -i 1 --output-format=json --output-file=pingstats.json 8.8.8.8`

Console output updates continuously with per-target stats, time series, and histograms; measurement runs until interrupted (Ctrl+C).

# Sample Output (representative)

Per-target stats table (live refresh):

```
Target        Sent  Recv  Loss   Min   Mean  Median  Max   Jitter
8.8.8.8       120   120   0.0%  18.3  19.7   19.4  24.1   1.2
1.1.1.1       120   118   1.7%  17.9  20.5   20.0  26.8   1.5
example.org    60    55   8.3%  25.4  33.1   31.8  48.7   3.6
```

Time-series text graph (recent samples):

```
1.1.1.1  ms:  |****    ** *   **  *  ** *
8.8.8.8  ms:  | ***  **  * **  ** *  *  **
example  ms:  |   **  * *   *  **   *   *
           t:  -10s                 now
```

Histogram buckets (latency distribution):

```
8.8.8.8 latency histogram (ms)
0-10   : ### (12)
10-20  : ################ (63)
20-30  : ####### (25)
30-40  : # (3)
40-50  : # (1)
```

# Architecture (high level)

- [`PingSession`](src/ping_session.hpp:1): manages periodic pings per target and forwards results.
- [`StatisticsAggregator`](src/statistics_aggregator.hpp:1): computes metrics, histograms, and time-series buffers.
- [`ConsoleView`](src/console_view.hpp:1): renders tables/graphs for the console.
- [`PlatformPingBackend`](src/platform_ping_backend.hpp:1): OS-specific ICMP implementation (factory-selected).
- Exporters: [`csv_exporter`](src/csv_exporter.cpp:1), [`json_exporter`](src/json_exporter.cpp:1).
- Target configuration: [`TargetConfig`](include/config.hpp:15) captures per-target settings (interval, output format/file, histogram buckets).

# Notes

- ICMP may require elevated privileges depending on OS (root/CAP_NET_RAW on Linux/WSL, admin on Windows).
- Doxygen docs: target `doxygen` is available when Doxygen is installed (`cmake --build <build> --target doxygen`).

# License

MIT License
