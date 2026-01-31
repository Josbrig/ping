# Report: README restoration and sync
- Timestamp: 2026-01-31 17:01:00 UTC
- Reference prompt: [`docs/prompts/Prompt_20260131_165204_UpdateDocs.md`](docs/prompts/Prompt_20260131_165204_UpdateDocs.md:1)

## Changes
- [`README.md`](README.md:1): Restored detailed system requirements per platform (General, Linux, macOS, Windows MSVC/WSL/Cygwin/MinGW), added representative sample outputs (stats table, time-series text graph, histogram buckets), noted continuous run until interrupted, and included `TargetConfig` in architecture. Kept build/test matrices (Debug/Release/RelWithDebInfo), optional `doxygen`, and user script references.
- [`docs/documentation/mainpage.md`](docs/documentation/mainpage.md:1): Synced usage note about continuous run, pointed to README for build/test matrix and `doxygen` target.
- [`wiki/Home.md`](wiki/Home.md:1): Added continuous-run note and pointer to README for build/test matrices, optional `doxygen`, and user scripts.

## Notes
- No code changes; documentation only.
- Maintained alignment with existing build/test instructions and platform support.
