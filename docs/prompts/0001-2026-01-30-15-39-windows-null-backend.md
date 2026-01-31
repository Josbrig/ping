# Prompt Report 0001 – 2026-01-30 15:39 – Windows Null Backend causing 100% loss

## Observation
- Manual pings to `dan-o-mat` and `megagamer` work with 0% loss.
- Running `scripts/test_hil.bat` shows 100% packet loss (no RTTs) and endless output.

## Cause
- On Windows, `make_platform_ping_backend()` in [`src/platform_ping_backend_factory.cpp`](src/platform_ping_backend_factory.cpp:1) returns a `NullPingBackend` because only macOS (`__APPLE__`) and Linux (`__linux__`) are supported. On other platforms (Windows), a stub backend is used whose `send_ping` always returns `success=false`.

## Implication
- With the current backend, pingstats cannot send real ICMP pings on Windows; all targets show 100% loss regardless of actual reachability.

## Proposed Fix
1) Implement a Windows backend (e.g., using IcmpSendEcho or raw sockets) and add it to the factory guard (WIN32).
2) Alternatively, temporarily document running on Windows via WSL or using an external ping until a native backend is available.

## Next Steps (recommended)
- Priority: add backend support for Windows; extend the factory with a WIN32 branch.
- Until then: mark in README/scripts that on Windows only the stub is active and real pings do not work.
