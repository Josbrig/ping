#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "ping_session.hpp"
#include "statistics_aggregator_impl.hpp"

using namespace pingstats;
using Catch::Approx;

namespace {

class FakeBackend final : public PlatformPingBackend {
public:
    struct Entry {
        double rtt_ms;
        bool success;
    };

    explicit FakeBackend(std::vector<Entry> script)
        : script_(std::move(script)) {}

    void initialize() override {}
    void shutdown() override {}

    PingResult send_ping(std::string_view /*host*/, std::chrono::milliseconds /*timeout*/) override
    {
        const auto idx = counter_++;
        const auto pos = idx % script_.size();
        const auto& e = script_[pos];
        return PingResult{e.success, e.rtt_ms};
    }

private:
    std::vector<Entry> script_;
    std::size_t counter_{0};
};

}  // namespace

TEST_CASE("ping workflow aggregates multiple hosts via fake backends")
{
    auto aggregator = make_statistics_aggregator();

    // Host A: alternating success/fail with RTT 10ms
    auto backend_a = std::make_shared<FakeBackend>(std::vector<FakeBackend::Entry>{
        {10.0, true}, {0.0, false}, {12.0, true}
    });

    // Host B: always success, RTTs 30/40ms
    auto backend_b = std::make_shared<FakeBackend>(std::vector<FakeBackend::Entry>{
        {30.0, true}, {40.0, true}
    });

    TargetConfig cfg_a; cfg_a.host = "host-a"; cfg_a.interval_s = 0.01;
    TargetConfig cfg_b; cfg_b.host = "host-b"; cfg_b.interval_s = 0.01;

    auto session_a = make_ping_session(cfg_a, backend_a, aggregator);
    auto session_b = make_ping_session(cfg_b, backend_b, aggregator);

    session_a->start();
    session_b->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    session_a->stop();
    session_b->stop();

    const auto snaps = aggregator->snapshot_all();
    REQUIRE(snaps.size() == 2);

    const auto find = [&](const std::string& h) {
        auto it = std::find_if(snaps.begin(), snaps.end(), [&](const auto& s){ return s.host == h; });
        REQUIRE(it != snaps.end());
        return *it;
    };

    const auto snap_a = find("host-a");
    REQUIRE(snap_a.count > 0);
    REQUIRE(snap_a.max_ms >= 10.0);
    REQUIRE(snap_a.loss_ratio > 0.0);

    const auto snap_b = find("host-b");
    REQUIRE(snap_b.count > 0);
    REQUIRE(snap_b.loss_ratio == Approx(0.0));
    REQUIRE(snap_b.mean_ms >= 30.0);
}

