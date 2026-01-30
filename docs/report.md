# Build/Test Report

## Summary
- Resolved CMake test-linking issue: test targets no longer link against the executable target.
- Tests now link only to Catch2::Catch2WithMain and include required sources directly.

## Changed Files
- tests/CMakeLists.txt

## Current Status
- CMake previously failed with "Target pingstats of type EXECUTABLE may not be linked". The test CMake configuration is adjusted to avoid linking against the executable.
- Pending: reconfigure CMake, build Debug/Release, and rerun `ctest` to verify on Windows (MSVC).

## Recommended Commands
- `cmake -S . -B build`
- `cmake --build build --config Debug`
- `ctest --test-dir build --build-config Debug --output-on-failure`
- `cmake --build build --config Release`
- `ctest --test-dir build --build-config Release --output-on-failure`
