\mainpage pingstats

\section overview Project Overview
pingstats is a cross-platform ping statistics tool that unifies backend implementations for Linux, macOS, Windows, WSL, Cygwin, and MinGW. It periodically pings configured targets, aggregates latency and loss metrics, and exports results for downstream analysis.

\section features Key Features
- Cross-platform platform backends selected via a factory to match the host OS.
- Periodic ping sessions with configurable targets and intervals.
- Statistics aggregation: min, max, mean, median, packet loss, histogram buckets, and time-series buffering.
- Multiple output views: console table/graphs plus CSV and JSON exporters.
- Test coverage with unit and integration tests to validate statistical correctness and workflows.

\section usage Quick Usage
- Build with CMake: configure and build the `pingstats` target.
- Run the binary with targets and options such as `-i` for interval, `--output-format`, and `--output-file`.
- Use exporters to persist results as CSV or JSON for further processing.

\section docs Documentation Layout
- API reference is generated from headers in `include/` and sources in `src/`.
- Project introduction and usage guidance live here on the main page.
- Additional project context can be found in the top-level `README.md`.
