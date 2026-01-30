// Windows-specific PlatformPingBackend using IcmpSendEcho from iphlpapi.
#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <string_view>

namespace pingstats {

class WindowsPingBackend final : public PlatformPingBackend {
public:
    WindowsPingBackend() = default;
    ~WindowsPingBackend() override;

    void initialize() override;
    void shutdown() override;
    PingResult send_ping(std::string_view host, std::chrono::milliseconds timeout) override;

    WindowsPingBackend(const WindowsPingBackend&) = delete;
    WindowsPingBackend& operator=(const WindowsPingBackend&) = delete;
    WindowsPingBackend(WindowsPingBackend&&) noexcept = default;
    WindowsPingBackend& operator=(WindowsPingBackend&&) noexcept = default;

private:
    void resolve_host(std::string_view host, unsigned long& out_ipv4) const;

    void* icmp_handle_{nullptr};
    bool initialized_{false};
    bool wsa_started_{false};
};

} // namespace pingstats
