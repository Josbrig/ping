#pragma once

#include "platform_ping_backend.hpp"

#include <chrono>
#include <cstdint>
#include <string_view>

namespace pingstats {

// Linux-spezifische Implementation des PlatformPingBackend für ICMP-Echo.
// Thread-Safety: nicht threadsicher; Aufrufer synchronisieren parallele Zugriffe.
class LinuxPingBackend final : public PlatformPingBackend {
public:
    LinuxPingBackend();
    ~LinuxPingBackend() override;

    // Initialisiert Raw-Socket-Ressourcen; wirft bei Fehlern eine Ausnahme.
    void initialize() override;

    // Gibt Ressourcen frei; darf mehrfach aufgerufen werden.
    void shutdown() override;

    // Sendet einen ICMP-Echo an host und wartet bis timeout. Gibt Erfolg und RTT in Millisekunden zurück.
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

