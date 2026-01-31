\page src_overview Source Module Documentation

This page summarizes all source modules under `src/`, explaining intent, background, and behavior beyond what the code already states. It focuses on how components collaborate and clarifies return values and invariants.

## src/main.cpp – CLI orchestration and runtime wiring
- [throw_cli_error()](src/main.cpp:39): Normalizes CLI parsing failures by throwing `std::runtime_error`, ensuring a single error-handling path and consistent exit codes.
- [print_usage()](src/main.cpp:43): Emits concise usage info including options for interval and output formats; no return value because it solely writes to the provided stream.
- [parse_output_format()](src/main.cpp:54): Maps strings to `OutputFormat` enum, rejecting unknown values to fail fast before any side effects.
- [parse_arguments()](src/main.cpp:67): Parses all CLI flags/hosts, collecting defaults as optionals to be resolved later, and stops early for `--help`/`--version` to avoid unnecessary setup work.
- [CsvExporterLoop](src/main.cpp:124): Lightweight background task that periodically persists CSV snapshots; `start()` is idempotent to prevent duplicate threads, `stop()` joins the worker to avoid dangling writes.
- [run()](src/main.cpp:172): End-to-end program flow—parses options, materializes `TargetConfig` entries, constructs shared `StatisticsAggregator`, spawns per-host `PingSession` plus optional exporters and console view, and performs orderly shutdown, writing a final export when enabled. Returns process exit code to the C entry point.

## src/ping_session.cpp – Periodic ping execution
- [PingSession::PingSession()](src/ping_session.cpp:14): Stores immutable target config and shared backend/aggregator handles; no additional logic.
- [PingSessionImpl](src/ping_session.cpp:22): Concrete session with a worker thread and atomic state. Uses `interval_s_` as live-updatable cadence.
  - [start()](src/ping_session.cpp:33): Atomically guards against double-start and launches the worker loop; no return value because success is best-effort and idempotent.
  - [stop()](src/ping_session.cpp:42): Signals termination and joins the worker to guarantee backend/aggregator safety.
  - [set_interval()](src/ping_session.cpp:50): Clamps to a sane minimum (0.1s) to avoid overload; returns void since the effect is captured in shared state.
  - [get_target()](src/ping_session.cpp:57): Exposes the immutable target for introspection.
  - [run_loop()](src/ping_session.cpp:60): Core loop—computes timeout as 80% of the interval (bounded 100–5000 ms) to balance responsiveness and jitter, performs ping via backend, forwards measurements (or failure) to aggregator, and sleeps the remainder of the interval.
- [make_ping_session()](src/ping_session.cpp:94): Factory returning a unique session bound to a platform backend and aggregator.

## src/statistics_aggregator.cpp – Metrics collection and snapshots
- [clamp_non_negative()](src/statistics_aggregator.cpp:19): Normalizes negative RTTs to zero, preventing histogram and summary pollution.
- [StatisticsAggregatorImpl::add_sample()](src/statistics_aggregator.cpp:128): Tracks sent/success counts, updates min/max/mean, maintains a sliding median buffer (bounded), bins RTTs into configurable histogram buckets, and stores a recent RTT ring for sparkline rendering.
- [snapshot()](src/statistics_aggregator.cpp:163): Returns an immutable `StatisticsSnapshot` for one host; if unseen, returns an empty snapshot to avoid exceptions.
- [snapshot_all()](src/statistics_aggregator.cpp:175): Copies internal state under lock, then builds snapshots lock-free to minimize contention during rendering/export.
- [reset()](src/statistics_aggregator.cpp:194) / [reset_all()](src/statistics_aggregator.cpp:204): Clear accumulated statistics (counts, histograms, buffers) for one or all hosts; void return because clearing is deterministic.
- [make_statistics_aggregator()](src/statistics_aggregator.cpp:212): Factory that hides implementation storage behind the interface type.

## src/console_view_impl.cpp – Terminal rendering
- [format_time_now()](src/console_view_impl.cpp:27): Builds a human-readable timestamp with local time; used in headers only.
- [pad_right()](src/console_view_impl.cpp:44): Column-aligned string formatting for tables.
- [format_ms()](src/console_view_impl.cpp:51): Fixed-precision milliseconds formatting; reused across table and histogram labeling.
- [ConsoleViewImpl](src/console_view_impl.cpp:65): Console renderer that owns a render thread guarded by `start_stop_mutex_`.
  - [render_header()](src/console_view_impl.cpp:75): Clears screen and prints title/timestamp banner.
  - [render_table()](src/console_view_impl.cpp:149): Tabulates per-host KPIs (count, loss, min/max/mean/median, last RTT) with `-` placeholders when data is absent, ensuring the view remains readable while warming up.
  - [render_time_series()](src/console_view_impl.cpp:190): Shows sparkline of recent RTTs per host, down-sampling when necessary to fit a fixed width.
  - [render_histogram()](src/console_view_impl.cpp:206): Renders bucketized RTT distribution with proportional bars normalized to the busiest bucket.
  - [render_once()](src/console_view_impl.cpp:245): Single-frame render pipeline: snapshot, sort by host, then render header/table/time-series/histogram.
  - [render_periodic()](src/console_view_impl.cpp:260): Spawns a periodic render loop; idempotent start.
  - [stop()](src/console_view_impl.cpp:275): Signals the render thread and joins to prevent dangling console writes.
- [make_console_view()](src/console_view_impl.cpp:287): Factory returning a shared console view bound to the aggregator.

## src/csv_exporter.cpp – CSV persistence
- [write_snapshots_csv_append()](src/csv_exporter.cpp:31): Appends timestamped statistics to a CSV, auto-creating the header on first write. Skips work when there are no snapshots, and throws if the file cannot be opened—callers handle/log errors.

## src/json_exporter.cpp – JSON persistence
- [write_snapshots_json()](src/json_exporter.cpp:55): Writes a full JSON document with timestamp, host list, histogram buckets, and recent RTTs. Truncates the file each time to provide a clean current snapshot.

## src/platform_ping_backend_factory.cpp – Backend selection
- [make_platform_ping_backend()](src/platform_ping_backend_factory.cpp:28): Chooses the concrete backend for the build target (Linux/macOS/Windows) and falls back to a null backend elsewhere, isolating platform specifics behind one factory.

## src/platform_ping_backend_linux.cpp – Raw ICMP (Linux)
- [checksum()](src/platform_ping_backend_linux.cpp:30): Internet checksum helper for ICMP packets.
- [initialize()](src/platform_ping_backend_linux.cpp:54): Opens a raw ICMP socket and seeds identifier/sequence; throws when CAP_NET_RAW/root is missing.
- [shutdown()](src/platform_ping_backend_linux.cpp:68): Closes the socket and clears state; void return because the close is best-effort.
- [send_ping()](src/platform_ping_backend_linux.cpp:76): Resolves host, crafts ICMP Echo request, measures RTT around send/receive, validates Echo Reply fields, and returns `{success,rtt_ms}`; uses a bounded timeout and yields failure on timeouts or malformed replies.

## src/platform_ping_backend_macos.cpp – ICMP datagram (macOS)
- [checksum()](src/platform_ping_backend_macos.cpp:30): Internet checksum for macOS ICMP header layout.
- [initialize()](src/platform_ping_backend_macos.cpp:54): Opens an ICMP datagram socket (root/entitlement required), seeds identifier/sequence.
- [shutdown()](src/platform_ping_backend_macos.cpp:68): Closes socket and resets flags.
- [send_ping()](src/platform_ping_backend_macos.cpp:76): Resolves host, builds Echo request, sends/awaits reply with `select`, validates Echo Reply identifiers, and returns `{success,rtt_ms}` with timeout mapped to failure.

## src/platform_ping_backend_windows.cpp – ICMP via WinAPI
- [resolve_host()](src/platform_ping_backend_windows.cpp:67): Resolves host to IPv4 using `InetPtonA` first, then `getaddrinfo`; throws on failure to avoid ambiguous ICMP errors.
- [initialize()](src/platform_ping_backend_windows.cpp:40): Starts Winsock, creates an ICMP handle; throws on startup errors and records WSA lifecycle for cleanup.
- [shutdown()](src/platform_ping_backend_windows.cpp:59): Closes ICMP handle; void return because cleanup is idempotent.
- [send_ping()](src/platform_ping_backend_windows.cpp:86): Uses `IcmpSendEcho` to emit an ICMP Echo, measures RTT as both API-reported and wall-clock (choosing API RTT when present), and maps `IP_REQ_TIMED_OUT` to a clean failure result; other errors throw `std::system_error` for diagnostics.
