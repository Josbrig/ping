#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <cstdint>
#include <string_view>

namespace pingstats {

/// Linux-specific implementation of PlatformPingBackend for ICMP echo.
/// Thread-safety: not thread-safe; callers must synchronize concurrent access.
class LinuxPingBackend final : public PlatformPingBackend {
public:
    LinuxPingBackend();
    ~LinuxPingBackend() override;

    /// Initializes raw-socket resources; throws on errors.
    void initialize() override;

    /// Releases resources; may be called multiple times.
    void shutdown() override;

    /// Sends an ICMP echo to host and waits until timeout. Returns success and RTT in milliseconds.
    PingResult send_ping(std::string_view host, std::chrono::milliseconds timeout) override;

    LinuxPingBackend(const LinuxPingBackend&) = delete;
    LinuxPingBackend& operator=(const LinuxPingBackend&) = delete;
    LinuxPingBackend(LinuxPingBackend&&) noexcept = default;
    LinuxPingBackend& operator=(LinuxPingBackend&&) noexcept = default;

private:
    int sock_fd_{-1};
    std::uint16_t identifier_{0};
    std::uint16_t sequence_{0};
    bool initialized_{false};
};

} // namespace pingstats

