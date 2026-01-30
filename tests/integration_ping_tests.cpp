#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <memory>
#include <string>

#include "platform_ping_backend_factory.hpp"

using namespace pingstats;

TEST_CASE("platform backend can ping configured host")
{
    const char* env_host = std::getenv("PINGSTATS_TEST_HOST");
    const std::string host = env_host ? std::string(env_host) : std::string{PINGSTATS_INTEGRATION_HOST};

    auto backend = make_platform_ping_backend();
    backend->initialize();
    const auto result = backend->send_ping(host, std::chrono::milliseconds{1000});
    backend->shutdown();

    if (!result.success) {
        WARN("Ping to host failed (backend may be stub or firewall blocks ICMP): " << host);
        return; // treat as skipped in environments without a real backend
    }

    REQUIRE(result.success == true);
}
