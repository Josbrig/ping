# pingstats – Project Wiki Home

Welcome to the pingstats wiki. This page summarizes what the project does, how to build and run it, and where to find detailed documentation.

## Overview
- Cross-platform C++20/CMake ping tool for multiple targets, capturing latency and availability over time.
- Per-target metrics: min/mean/median/max, loss, histograms, time series; live console views.
- Platform backends for Linux, macOS, Windows (MSVC), WSL, Cygwin, MinGW via an extensible abstraction layer.

## Quick Start
### Build (Unix-like: Linux/macOS/WSL)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
For debug builds use `-DCMAKE_BUILD_TYPE=Debug`.

### Build (Windows Cygwin / MinGW)
```bash
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
Adjust generator (e.g., Ninja) with `-G` if desired.

### Run
```bash
./build/pingstats [options] <target1> [target2 ...]
```
Examples:
- Single target: `./build/pingstats 8.8.8.8`
- Multiple targets: `./build/pingstats 8.8.8.8 1.1.1.1 example.org`
- CSV export: `./build/pingstats -i 1 --output-format=csv --output-file=pingstats.csv 8.8.8.8 1.1.1.1`
- JSON export: `./build/pingstats -i 1 --output-format=json --output-file=pingstats.json 8.8.8.8`
- Custom interval (seconds): `./build/pingstats -i 2 8.8.8.8 1.1.1.1`
- Write CSV: `./build/pingstats -i 1 --output-format=csv --output-file=pingstats.csv 8.8.8.8`
- Measurement runs continuously until interrupted (Ctrl+C).

## Features
- Continuous ICMP echo measurements per target with configurable intervals.
- Live console rendering with tables, time series, and latency histograms.
- CSV/JSON export of aggregated statistics.
- Modular platform backends and statistics pipeline for easy extension.

## Architecture
See the detailed architecture write-up in [`docs/architecture.md`](docs/architecture.md:1). Key components:
- Backend abstraction: [`PlatformPingBackend`](include/platform_ping_backend.hpp:1) with OS implementations (e.g., [`platform_ping_backend_linux.hpp`](include/platform_ping_backend_linux.hpp:1), [`platform_ping_backend_macos.hpp`](include/platform_ping_backend_macos.hpp:1)).
- Session logic: [`ping_session.hpp`](include/ping_session.hpp:1) drives per-target workers and hands results to the aggregator.
- Statistics: [`statistics_aggregator.hpp`](include/statistics_aggregator.hpp:1) maintains counters, histograms, and time series.
- Console UI: [`console_view.hpp`](include/console_view.hpp:1) renders tables, series, and histograms.
- Entry point: [`main.cpp`](src/main.cpp:1) wires configuration, backends, sessions, and the view.
- Build/test matrices (Debug/Release/RelWithDebInfo), optional `doxygen` target, and user scripts are documented in [`README.md`](README.md:1).

## Testing
Build then run the CTest suite from the build directory:
```bash
ctest --output-on-failure
```
On multi-config generators (e.g., MSVC), use `--build-config Debug` or `Release`.

## Further Reading
- Project overview and usage details: [`README.md`](README.md:1)
- Internal rules for agents and docs: [`docs/AGENTEN_RULES.md`](docs/AGENTEN_RULES.md:1)
- Reports and prompts: [`docs/reports/`](docs/reports/:1) and [`docs/prompts/`](docs/prompts/:1)
- TODOs and plans: [`docs/todo/`](docs/todo/:1) and [`plans/`](plans/:1)

## Contributing
Follow standard fork/branch/PR workflow and keep changes tested. See contribution notes in [`README.md`](README.md:214).
