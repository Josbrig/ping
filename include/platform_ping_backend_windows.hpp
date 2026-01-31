/// Windows-specific PlatformPingBackend using IcmpSendEcho from iphlpapi.
#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <string_view>

namespace pingstats {

class WindowsPingBackend final : public PlatformPingBackend {
public:
    WindowsPingBackend() = default;
    ~WindowsPingBackend() override;

    /// Initialize Winsock and ICMP handle; throws on errors.
    void initialize() override;
    /// Close ICMP handle; idempotent.
    void shutdown() override;
    /// Send ICMP Echo via WinAPI; maps timeouts to {false,0} and throws on other errors.
    PingResult send_ping(std::string_view host, std::chrono::milliseconds timeout) override;

    WindowsPingBackend(const WindowsPingBackend&) = delete;
    WindowsPingBackend& operator=(const WindowsPingBackend&) = delete;
    WindowsPingBackend(WindowsPingBackend&&) noexcept = default;
    WindowsPingBackend& operator=(WindowsPingBackend&&) noexcept = default;

private:
    /// Resolve host to IPv4, trying literal parsing before DNS; throws on failure.
    void resolve_host(std::string_view host, unsigned long& out_ipv4) const;

    void* icmp_handle_{nullptr};
    bool initialized_{false};
    bool wsa_started_{false};
};

} // namespace pingstats
