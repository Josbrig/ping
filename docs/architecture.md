# Architecture Overview for pingstats

## Short Context

pingstats is a C++20 console tool, built with CMake, for analyzing network latency. The architecture is cross-platform and supports Linux, macOS, WSL, Cygwin, and MinGW. Low-level details such as ICMP handling and socket APIs are encapsulated behind clearly defined backend interfaces.

## Main Modules and Responsibilities

### PlatformPingBackend

- Implements OS-specific creation and evaluation of ICMP echo requests and replies.
- Encapsulates differences in underlying network APIs per platform (raw sockets, permissions, timeouts).
- Provides a unified interface so higher layers can issue ping requests for a target and timestamp and receive the measured round-trip time (RTT).
- Enables swapping or adding more backends without changing the rest of the application.

### PingSession and PingWorker

- Represent the ongoing measurement for a target (host or IP) at a fixed interval.
- PingSession manages the lifecycle for a target (start, stop, error handling) and holds references to PlatformPingBackend and StatisticsAggregator.
- PingWorker performs the actual ping operations on a recurring schedule (e.g., its own thread per target or from a thread pool).
- Each measurement produces a raw data result with timestamp, RTT, success status, and sequence number, which is passed to the StatisticsAggregator.

### StatisticsAggregator

- Receives raw measurements from all active PingSessions.
- Computes rolling metrics such as minimum, maximum, arithmetic mean, and median RTT.
- Maintains a histogram of latency ranges and a time-series buffer for the history of measurements.
- Derives packet loss by comparing sent vs. successfully answered packets per target.
- Provides aggregated views per target and optionally global views for the ConsoleView and export functions.

### TargetConfig and Config

- Describe targets defined statically or loaded from configuration files, including hostnames/IPs, ping intervals, and runtime settings.
- Define formatting options for output and export (e.g., enabled metrics, histogram resolution, time-series length).
- Serve main as the central source for all configurations so modules like PingSession, StatisticsAggregator, and ConsoleView are initialized consistently.

### ConsoleView and Renderer

- Visualize current statistics in the console.
- Build tabular overviews per target with metrics such as Min/Max/Mean/Median, packet loss, and current RTT.
- Present time-series views (e.g., compact sparklines or simplified timelines) based on the StatisticsAggregator’s time-series buffer.
- Render histograms of latency distribution in textual form.
- Refresh the console periodically or event-driven by fetching the latest data from the StatisticsAggregator.

### main

- Entry point of the application and central “wiring” of the modules.
- Performs CLI parsing (targets, intervals, output options, export formats) and derives TargetConfig and global Config.
- Initializes PlatformPingBackend and StatisticsAggregator.
- Creates a PingSession and corresponding PingWorker for each configured target and starts parallel execution.
- Initializes ConsoleView and sets up periodic refresh, typically via its own rendering loop or timer.
- Coordinates clean shutdown, stops sessions, and ensures a consistent termination of all background activities.

## Data Flows

- PingWorkers in each PingSession generate measurement events for their target by sending ICMP requests via PlatformPingBackend and evaluating responses.
- Every successful or failed measurement is sent as an event to the StatisticsAggregator.
- The StatisticsAggregator updates its internal structures (statistics, histograms, time-series buffers, packet-loss counters).
- ConsoleView periodically pulls the current aggregation data from the StatisticsAggregator and renders the console output.
- Export functions (e.g., CSV/JSON) also read from the data provided by the StatisticsAggregator without interfering with the measurement flow.

The data flows are predominantly event-based (measurement events from sessions to the aggregator) and are complemented by periodic pulling of aggregated data by the view and export paths.

## Concurrency and Synchronization

- Each target can run its own PingSession with a dedicated PingWorker thread; alternatively, a thread pool can serve multiple targets.
- All PingWorkers write concurrently into the shared StatisticsAggregator.
- The StatisticsAggregator uses appropriate synchronization mechanisms (e.g., mutexes or lock-free structures where appropriate) to ensure consistent updates to statistical data structures.
- Read access by ConsoleView and export functions typically occurs in parallel with measurement writes and is safeguarded by the same synchronization primitives.
- Configuration (TargetConfig/Config) is treated as largely immutable after initialization and can generally be read without additional synchronization.

## Extensibility

- PlatformPingBackend is designed as a swap point for different ICMP implementations. Additional backends can implement specialized system APIs or user-space pings (UDP/TCP) without requiring changes to PingSession or StatisticsAggregator.
- ConsoleView and Renderer are structured to allow additional output targets, such as colored console output, alternative layouts, or other rendering formats.
- The export layer supports CSV and JSON outputs based on the data provided by the StatisticsAggregator. Further export paths, such as Prometheus endpoints or a web UI, can be added by consuming the same aggregation data.
- New metrics or additional analyses can be implemented in the StatisticsAggregator without fundamentally altering the measurement path, as long as the interfaces for raw measurements remain stable.

