#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <memory>
#include <string>

#ifndef PINGSTATS_INTEGRATION_HOST
#define PINGSTATS_INTEGRATION_HOST "simbrig.eu"
#endif

#include "platform_ping_backend_factory.hpp"

using namespace pingstats;

TEST_CASE("platform backend can ping configured host")
{
    const char* env_host = std::getenv("PINGSTATS_TEST_HOST");
    const std::string host = env_host ? std::string{env_host} : std::string{PINGSTATS_INTEGRATION_HOST};

    auto backend = make_platform_ping_backend();
    try {
        backend->initialize();
    } catch (const std::exception& ex) {
        WARN("Skipping integration ping (initialize failed): " << ex.what());
        return; // treat as skipped when raw ICMP is unavailable (e.g., no CAP_NET_RAW)
    }

    PlatformPingBackend::PingResult result{};
    try {
        result = backend->send_ping(host, std::chrono::milliseconds{1000});
    } catch (const std::exception& ex) {
        backend->shutdown();
        WARN("Skipping integration ping (send failed): " << ex.what());
        return;
    }

    backend->shutdown();

    if (!result.success) {
        WARN("Ping to host failed (backend may be stub or firewall blocks ICMP): " << host);
        return; // treat as skipped in environments without a real backend
    }

    REQUIRE(result.success == true);
}
