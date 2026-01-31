#include "ping_session.hpp"
#include "platform_ping_backend.hpp"
#include "statistics_aggregator.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

namespace pingstats {

PingSession::PingSession(TargetConfig target,
                         std::shared_ptr<PlatformPingBackend> backend,
                         std::shared_ptr<StatisticsAggregator> aggregator)
    : target_(std::move(target)), backend_(std::move(backend)), aggregator_(std::move(aggregator))
{}

namespace {

/// Concrete ping session that owns a worker thread and supports live interval updates.
class PingSessionImpl final : public PingSession {
public:
    PingSessionImpl(TargetConfig target,
                    std::shared_ptr<PlatformPingBackend> backend,
                    std::shared_ptr<StatisticsAggregator> aggregator)
        : PingSession(std::move(target), std::move(backend), std::move(aggregator)),
          interval_s_(target_.interval_s.value_or(1.0))
    {}

    ~PingSessionImpl() override { stop(); }

    /// Start the periodic ping loop if not already running.
    void start() override
    {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            return;
        }
        worker_ = std::thread(&PingSessionImpl::run_loop, this);
    }

    /// Stop the loop and join the worker to ensure backend/aggregator safety.
    void stop() override
    {
        running_.store(false);
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    /// Update the interval (seconds), clamped to avoid excessive request rates.
    void set_interval(double seconds) override
    {
        constexpr double kMinInterval = 0.1;
        const double clamped = std::max(seconds, kMinInterval);
        interval_s_.store(clamped);
    }

    [[nodiscard]] const TargetConfig& get_target() const override { return target_; }

private:
    /// Worker loop: performs ping, records results, and sleeps remaining interval.
    void run_loop()
    {
        while (running_.load()) {
            const auto iteration_start = std::chrono::steady_clock::now();
            const double interval = interval_s_.load();
            const double timeout_ms_d = std::clamp(interval * 800.0, 100.0, 5000.0);
            const auto timeout = std::chrono::milliseconds(static_cast<int>(timeout_ms_d));

            try {
                const auto result = backend_->send_ping(target_.host, timeout);
                aggregator_->add_sample(target_.host, result.rtt_ms, result.success);
            } catch (const std::exception& ex) {
                std::cerr << "PingSession error for " << target_.host << ": " << ex.what() << std::endl;
                aggregator_->add_sample(target_.host, 0.0, false);
            } catch (...) {
                std::cerr << "PingSession unknown error for " << target_.host << std::endl;
                aggregator_->add_sample(target_.host, 0.0, false);
            }

            const auto elapsed = std::chrono::steady_clock::now() - iteration_start;
            const auto remaining = std::chrono::duration<double>(interval) - std::chrono::duration<double>(elapsed);
            if (remaining.count() > 0) {
                std::this_thread::sleep_for(remaining);
            }
        }
    }

    std::atomic<bool> running_{false};
    std::atomic<double> interval_s_;
    std::thread worker_;
};

}  // namespace

std::unique_ptr<PingSession> make_ping_session(TargetConfig target,
                                               std::shared_ptr<PlatformPingBackend> backend,
                                               std::shared_ptr<StatisticsAggregator> aggregator)
{
    return std::make_unique<PingSessionImpl>(std::move(target), std::move(backend), std::move(aggregator));
}

}  // namespace pingstats

