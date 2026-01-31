# Report: Updated README and project descriptions
- Timestamp: 2026-01-31 16:52:55 UTC
- Purpose: Ensure README and related descriptions reflect current build/test matrices, doc generation, and platform support.

## Changes
- [`README.md`](README.md:1): Rewrote overview, features, build/test matrix (Debug/Release/RelWithDebInfo), doc generation (doxygen target), and linked user build scripts per platform (scripts/user/*). Simplified architecture section to match actual code components.
- [`docs/documentation/mainpage.md`](docs/documentation/mainpage.md:1): Updated overview/features/usage to align with current capabilities and platforms.
- [`wiki/Home.md`](wiki/Home.md:1): Synced overview and build/run examples; added CSV/JSON export examples.

## Notes
- Doxygen target remains optional; built when Doxygen is present.
- Architecture description now matches existing components (PingSession, StatisticsAggregator, ConsoleView, PlatformPingBackend, exporters).

## TODO impact
- No TODO items modified.

## References
- Prompt archive: [`docs/prompts/Prompt_20260131_165204_UpdateDocs.md`](docs/prompts/Prompt_20260131_165204_UpdateDocs.md:1)
