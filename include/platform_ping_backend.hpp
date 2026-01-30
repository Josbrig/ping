#pragma once

#include <chrono>
#include <string_view>

namespace pingstats {

// Abstract backend for platform-specific ICMP operations.
// Thread-safety: instances may be used from multiple threads only if derived classes guarantee it.
class PlatformPingBackend {
public:
    struct PingResult {
        bool success;
        double rtt_ms;
    };

    virtual ~PlatformPingBackend() = default;

    PlatformPingBackend() = default;
    PlatformPingBackend(const PlatformPingBackend&) = delete;
    PlatformPingBackend& operator=(const PlatformPingBackend&) = delete;
    PlatformPingBackend(PlatformPingBackend&&) = default;
    PlatformPingBackend& operator=(PlatformPingBackend&&) = default;

    // Optional lifecycle hook to prepare resources (sockets, permissions).
    // Thread-safety: call during single-threaded setup.
    virtual void initialize() = 0;

    // Optional lifecycle hook to release resources.
    // Thread-safety: ensure no concurrent send_ping calls when invoked.
    virtual void shutdown() = 0;

    // Send a single ping to the given host with timeout.
    // Thread-safety: implementation defines reentrancy; typically one call at a time per instance.
    virtual PingResult send_ping(std::string_view host, std::chrono::milliseconds timeout) = 0;
};

}  // namespace pingstats

