```markdown
# Concept: Histogram Bucket Configuration (Build Time and Runtime)

## Objective
- Support two ways to define histogram buckets:
  1. **Build/Compile time**: a default file in the repository (built/packaged) providing default buckets for all targets.
  2. **Runtime/CLI**: parameters at startup to set buckets per invocation.
- Clear precedence: CLI > compile-time config > hardcoded fallback default.

## Current State
- Fixed default `{10,20,50,100,200,500}` in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17); hosts receive this set when created [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
- Buckets are applied/rendered on snapshot [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:109) and shown in the console [`console_view_impl.cpp`](src/console_view_impl.cpp:212); export uses the same values [`json_exporter.cpp`](src/json_exporter.cpp:78).
- CLI has no bucket option; `TargetConfig.bucket_boundaries` exists but is never set [`config.hpp`](include/config.hpp:15) [`main.cpp`](src/main.cpp:198).
- PingSession takes TargetConfig as-is; buckets must be finalized before session creation [`ping_session.cpp`](src/ping_session.cpp:14).

## Requirements and Assumptions
- Inputs: ascending, non-empty list of non-negative doubles.
- Limit bucket count (e.g., max 64); remove/reject duplicates.
- Invalid input leads to a clear CLI error message and exit code ≠ 0; no silent fallback unless the option is absent.
- Thread safety: buckets are read only at host creation; existing mutex logic is sufficient as long as buckets are final before sessions start.

## Path 1: Compile-Time Configuration via File
- **File**: `config/buckets_default.json` (or `.yaml`), maintained in-repo and copied/installed by the build (CMake) into the output directory.
- **Format (JSON example)**:
  ```json
  {
    "bucket_boundaries": [0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000]
  }
  ```
- **Load point**: startup in `main` before target creation:
  - Derive path relative to the executable (e.g., `./config/buckets_default.json`) or override via `--config-file`.
  - On successful load/validation, set these boundaries as the global default.
  - If missing/parse error → warn and fall back to the hardcoded default (no crash).
- **Integration**:
  - Utility `load_bucket_defaults_from_file(path) -> std::vector<double>` with validation.
  - Add optional parameter `--config-file <path>` in `parse_arguments` (initially for bucket defaults, extendable).
  - When building `TargetConfig` (before `make_ping_session`), adopt the loaded set unless a CLI override is provided.

## Path 2: Runtime/CLI Override
- **Option**: `--bucket-boundaries <list|preset>`
  - List: comma-separated, e.g., `--bucket-boundaries 0.5,1,2,5,10,20,50,100,200,500,1000`
  - Presets (optional): `fine`, `log`, `coarse`; mapped to predefined tables.
- **Parsing**:
  - Split by comma, use `std::stod`, reject negatives, enforce ascending order (or sort and deduplicate with a warning).
  - Enforce max length (e.g., 64 elements); reject empty lists.
- **Application**:
  - Apply the CLI value globally per process run (same set for all targets); if per-host is needed later, CLI could support multiple uses—currently not required.
  - Precedence: if the option is set, it overrides the compile-time file and hardcoded fallback.
- **Error cases**:
  - Invalid number/order → error message and exit.
  - Too many buckets → error message and exit.

## Validation and Error Handling
- Shared validator for all sources (CLI, file):
  - Non-empty, strictly ascending, no `NaN/Inf`, no negatives, enforce max length.
- Logging/warnings: missing file → warn and fallback; CLI errors → abort with message.

## Thread Safety
- Buckets are used only during host setup; as long as configuration is finalized before sessions start, existing mutex synchronization in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:122) is sufficient.
- No runtime change is planned; no re-binning needed.

## Open Points
- Path strategy for `config/buckets_default.json` across Windows/macOS/Linux (clarify install/run layout).
- Whether presets are required or only free-form lists.
- Whether per-host buckets are needed later.

## Implementation Steps (architecture-oriented)
- Extend CLI (`parse_arguments`): new option `--bucket-boundaries`; optional `--config-file`.
- Implement loader/validator (file + CLI) and define fallback cascade (CLI > file > default).
- Populate `TargetConfig` with final boundaries during construction; initialize `StatisticsAggregator` with them.
- Tests: parsing/validation, aggregator with custom buckets, error scenarios.
```
