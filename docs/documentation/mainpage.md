\mainpage pingstats

\section overview Project Overview
pingstats is a cross-platform ping statistics tool (C++20/CMake) with backend implementations for Linux, macOS, Windows (MSVC), WSL, Cygwin, and MinGW. It periodically pings configured targets, aggregates latency and loss metrics, and exports results (CSV/JSON) for downstream analysis.

\section features Key Features
- Cross-platform backends selected via a factory per host OS.
- Periodic ping sessions with configurable targets and intervals.
- Statistics: min, max, mean, median, packet loss, histogram buckets, time-series buffering.
- Output: console tables/graphs plus CSV and JSON exporters.
- Tests: unit and integration tests via CTest.

\section usage Quick Usage
- Build with CMake (single-config or multi-config per platform/generator).
- Run `pingstats` with targets and options (`-i`, `--output-format`, `--output-file`).
- Export CSV/JSON for further processing.
- Measurements run continuously until interrupted (Ctrl+C).
- Full build/test matrix, platform requirements, and optional `doxygen` target are documented in [`README.md`](../README.md:1).

\section docs Documentation Layout
- API reference is generated from headers in `include/` and sources in `src/`.
- Project introduction and usage guidance live here on the main page.
- Additional project context can be found in the top-level `README.md`.
