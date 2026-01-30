#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <cstdint>
#include <string_view>

namespace pingstats {

// macOS-spezifische Implementation des PlatformPingBackend für ICMP-Echo.
// Thread-Safety: nicht threadsicher; parallele Zugriffe müssen extern synchronisiert werden.
class MacOsPingBackend final : public PlatformPingBackend {
public:
    MacOsPingBackend();
    ~MacOsPingBackend() override;

    void initialize() override;  // Bereitet Raw-Socket vor; erfordert root/CAP_NET_RAW-ähnliche Rechte.
    void shutdown() override;    // Gibt Ressourcen frei.

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

