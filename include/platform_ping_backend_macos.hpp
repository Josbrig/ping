#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <cstdint>
#include <string_view>

namespace pingstats {

// macOS-specific implementation of PlatformPingBackend for ICMP echo.
// Thread-safety: not thread-safe; callers must synchronize concurrent access externally.
class MacOsPingBackend final : public PlatformPingBackend {
public:
    MacOsPingBackend();
    ~MacOsPingBackend() override;

    void initialize() override;  // Prepares raw socket; requires root/CAP_NET_RAW-like privileges.
    void shutdown() override;    // Releases resources.

    PingResult send_ping(std::string_view host, std::chrono::milliseconds timeout) override;

    MacOsPingBackend(const MacOsPingBackend&) = delete;
    MacOsPingBackend& operator=(const MacOsPingBackend&) = delete;
    MacOsPingBackend(MacOsPingBackend&&) noexcept = default;
    MacOsPingBackend& operator=(MacOsPingBackend&&) noexcept = default;

private:
    int sock_fd_{-1};
    std::uint16_t identifier_{0};
    std::uint16_t sequence_{0};
    bool initialized_{false};
};

} // namespace pingstats

