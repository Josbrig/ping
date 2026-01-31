```markdown
# Project Overview

This project provides a cross-platform ping tool that collects long-term statistics on the reachability and response times of one or more network targets. The goal is reliable, continuous monitoring with meaningful metrics and visualizations of latency over extended periods of time.

The program is implemented in C++20 and built with CMake. It is designed to compile and run on Linux, macOS, Windows WSL, Windows Cygwin, and Windows MinGW. Platform-specific requirements for ICMP packets on each operating system are taken into account.

# Key Features

- Continuous sending of ICMP echo requests (pings) to one or more targets at configurable intervals.
- Collection and analysis of the response time of each individual ping reply.
- Per-target statistics for each IP address or hostname (separate evaluation per destination).
- Display of the response times over time (e.g., as a simple text graph / latency curve in the console).
- Creation of a histogram of ping response times to visualize the distribution of latencies.
- Calculation of minimum, maximum, mean, and median per target based on all observed responses so far.
- Ongoing measurement and statistics updates in near real time: the console is refreshed at regular intervals as long as the program is running.
- Simultaneous monitoring of multiple targets: each target is measured in its own session and clearly separated in the output.

# System Requirements

## General

- C++20-capable compiler (e.g., GCC, Clang, MSVC, or MinGW with C++20 support).
- CMake in a recent version, recommended 3.20 or later.
- Network access to the monitored targets.
- Permissions to send ICMP echo requests (ping). Depending on platform and configuration, elevated privileges may be required (e.g., root on Linux, administrator privileges on Windows, or appropriate capabilities such as `CAP_NET_RAW`).

## Linux

- Linux distribution with an installed C++20 compiler (e.g., `g++` or `clang++`).
- CMake ≥ 3.20.
- Sufficient ICMP permissions (e.g., running with `sudo` or setting `CAP_NET_RAW` on the binary).

## macOS

- Recent Xcode toolchain or Command Line Tools with C++20 support.
- CMake ≥ 3.20 (installed, for example, via Homebrew or MacPorts).
- Execution in a terminal environment with sufficient privileges for ICMP packets.

## Windows WSL

- Windows with Windows Subsystem for Linux (WSL or WSL2) installed.
- Linux environment in WSL with a C++20 compiler and CMake ≥ 3.20.
- ICMP permissions inside the WSL distribution (possibly `sudo`).

## Windows Cygwin

- Cygwin installation with a C++20-capable compiler (e.g., `g++` from the Cygwin packages).
- CMake ≥ 3.20 as a Cygwin package.
- Execution in a Cygwin terminal with the permissions required for ICMP.

## Windows MinGW

- MinGW or MSYS2 environment with a C++20-capable compiler (e.g., `g++` from MinGW or `clang++`).
- CMake ≥ 3.20 (e.g., as an MSYS2 package).
- Execution in the corresponding shell (MSYS2/MinGW) with network access and, if necessary, administrator privileges if ICMP requires them.

# Installation

The following steps describe a typical CMake-based build. The exact process may vary depending on the environment (generator, paths, compiler), but the basic principle remains the same.

## Unix-like Systems (Linux, macOS, WSL)

In the project directory:

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For a debug build, adjust the build type accordingly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Depending on the chosen generator (e.g., Ninja, Makefiles), the appropriate build tools are invoked automatically.

## Windows (Cygwin / MinGW)

In the project directory, from a Cygwin or MSYS2/MinGW shell:

```bash
mkdir -p build
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For debug builds:

```bash
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

If a different generator (e.g., Ninja) is preferred, it can be specified via `-G`.

# Usage

In the following, we assume that the resulting binary is called [`pingstats`](build/pingstats) and is located in the `build` directory after the build.

The general invocation pattern is:

```bash
./build/pingstats [options] <target1> [target2 ...]
```

Typical targets are IP addresses or hostnames, such as `8.8.8.8` or `example.org`.

## Simple Example: Ping a Single Target

Continuous pinging of a single target with the default interval (e.g., 1 second):

```bash
./build/pingstats 8.8.8.8
```

The measurement continues until the user explicitly stops it (e.g., with `Ctrl+C`). Statistics and display are updated continuously.

## Monitor Multiple Targets in Parallel

Simultaneous monitoring of multiple IP addresses or hosts:

```bash
./build/pingstats 8.8.8.8 1.1.1.1 example.org
```

An internal session is created for each target, and the output shows separate statistics per address/host.

## Specify the Interval in Seconds

With an option to control the interval (for example, `-i` in seconds):

```bash
./build/pingstats -i 2 8.8.8.8 1.1.1.1
```

In this example, all specified targets are pinged every 2 seconds. The interval directly influences the temporal resolution of the statistics.

## Write Output to a File (e.g., CSV or JSON)

For later analysis, statistics can be written periodically or at program termination to a file, for example in CSV or JSON format. One possible syntax (example):

```bash
./build/pingstats -i 1 --output-format=csv --output-file=pingstats.csv 8.8.8.8 1.1.1.1
```

or for JSON:

```bash
./build/pingstats -i 1 --output-format=json --output-file=pingstats.json 8.8.8.8 1.1.1.1
```

Depending on the implementation, files may be updated continuously or written once at the end of the measurement.

## Example Console Output

During runtime, the program displays continuously updated statistics, typically in tabular form. A simplified example might look as follows:

```text
Timestamp: 2026-01-30 12:34:56

Target         Packets   Min [ms]   Max [ms]   Mean [ms]   Median [ms]   Loss
-------------  -------  ---------  ---------  ----------  ------------  -----
8.8.8.8           120       12.3       25.8        14.7          13.9    0.0 %
1.1.1.1           120       10.8       21.2        13.5          12.7    0.0 %
example.org       120       18.5       60.2        22.1          20.4    0.8 %
```

In addition, a simple time series of the most recent measurements per target can be shown at regular intervals, for example as a text graph:

```text
Time series of the last 60 seconds (latency in ms, lower is better)

8.8.8.8:     ▁▁▂▂▃▃▄▅▆▆▆▇█▇▆▅▄▃▂▂▁▁
1.1.1.1:     ▁▁▁▂▂▂▃▄▅▅▆▆▇▇▆▅▄▃▂▂▁▁
example.org: ▂▃▃▄▅▆▇██▇▆▅▄▃▃▂▂▁▁▁▁▁
```

The histogram of response times can be printed, for example, as a bucket list:

```text
Histogram 8.8.8.8 (response time in ms)

  0–10   ms:  5 ▓▓
 10–20   ms: 80 ██████████████████
 20–30   ms: 25 █████
 30–40   ms: 10 ██
  > 40   ms:  0
```

By default, the measurement runs indefinitely and is terminated by the user. With each update cycle, new ping results are incorporated into the statistics, and the reported metrics (min/max/mean/median, time series, histogram) are adjusted dynamically.

# Project Architecture

The internal structure is designed so that measurement logic, statistics calculation, configuration, output, and operating system abstraction are clearly separated. This keeps the code maintainable, extensible, and well testable.

Core building blocks include the following components:

- [`PingSession`](src/ping_session.hpp:1) or [`PingWorker`](src/ping_worker.hpp:1): Responsible for executing ICMP pings to a single target. Controls the send interval, collects raw measurement data (response times, packet loss), and reports it to the statistics component.
- [`StatisticsAggregator`](src/statistics_aggregator.hpp:1): Receives raw data from individual sessions, computes minimum, maximum, mean, and median, maintains counters for packet loss, and produces data structures for histograms and time series (ring buffers for the last N measurements).
- [`TargetConfig`](src/config.hpp:1): Represents a single target, including IP/hostname, ping interval, and optional target-specific settings. A higher-level configuration instance manages the complete set of targets.
- [`ConsoleView`](src/console_view.hpp:1): Produces console output at regular intervals. Reads the current state from [`StatisticsAggregator`](src/statistics_aggregator.hpp:1) and renders tables, time series, and histograms, ideally without flooding the terminal with excessive scrolling (for example, by updating the existing screen area).
- [`PlatformPingBackend`](src/platform_ping_backend.hpp:1): Abstraction layer for the operating-system-specific ICMP implementation. Encapsulates differences in the available system calls, sockets, and privileges on Linux, macOS, WSL, Cygwin, and MinGW.

Program startup (for example, in a function [`main`](src/main.cpp:1)) reads the configuration and command-line arguments, creates a [`TargetConfig`](src/config.hpp:1) instance for each target, and launches the corresponding [`PingSession`](src/ping_session.hpp:1) instances, typically in separate threads or asynchronous tasks. Each session uses [`PlatformPingBackend`](src/platform_ping_backend.hpp:1) to send ICMP packets and measure response times, and forwards the results to [`StatisticsAggregator`](src/statistics_aggregator.hpp:1).

In parallel, [`ConsoleView`](src/console_view.hpp:1) periodically retrieves the current state from [`StatisticsAggregator`](src/statistics_aggregator.hpp:1) and refreshes the console display. This provides a continuously updated view of the runtime statistics for all monitored targets.

# Contributing / Extensibility

External contributions are welcome as long as they support the project’s objective: a robust, cross-platform, and easy-to-understand long-term ping statistics tool.

A typical contribution workflow can look as follows:

1. Fork the repository.
2. Create a dedicated branch for your change (e.g., `feature/...` or `bugfix/...`).
3. Implement your changes, including appropriate tests and/or manual verification (for example, across multiple platforms, where possible).
4. Align code style and structure with the existing codebase (clear separation of logic, statistics, output, and platform abstraction).
5. Open a merge request or pull request with a concise, informative description.

Potentially useful extensions include, among others:

- Exporting metrics in a Prometheus-compatible format (for example via a local HTTP endpoint or a text file) to feed the data into external monitoring systems.
- A web-based user interface that visualizes the statistics in real time in a browser (e.g., with charts for latency over time, histograms, and per-target availability).
- Additional console visualizations, such as percentile views (P95, P99), jitter calculations, or heat maps of latency over time.
- Support for additional protocols and checks beyond ICMP, e.g., TCP port reachability, HTTP GET latency measurements, or TLS handshake times.
- Configuration files (e.g., YAML or JSON) to conveniently manage a larger number of targets and settings.

# License

MIT License
```
