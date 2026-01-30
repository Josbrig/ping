#include "platform_ping_backend_factory.hpp"

#if defined(__APPLE__)
#include "platform_ping_backend_macos.hpp"
#elif defined(__linux__)
#include "platform_ping_backend_linux.hpp"
#endif

#include <memory>

namespace pingstats {

namespace {

class NullPingBackend final : public PlatformPingBackend {
public:
    void initialize() override {}
    void shutdown() override {}
    PingResult send_ping(std::string_view /*host*/, std::chrono::milliseconds /*timeout*/) override {
        return PingResult{false, 0.0};
    }
};

} // namespace

std::unique_ptr<PlatformPingBackend> make_platform_ping_backend() {
#if defined(__APPLE__)
    return std::make_unique<MacOsPingBackend>();
#elif defined(__linux__)
    return std::make_unique<LinuxPingBackend>();
#else
    return std::make_unique<NullPingBackend>();
#endif
}

} // namespace pingstats

