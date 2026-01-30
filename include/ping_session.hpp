#pragma once

#include <memory>

#include "config.hpp"

namespace pingstats {

class PlatformPingBackend;
class StatisticsAggregator;

// Manages the lifecycle of pinging a single target.
// Thread-safety: control methods should be synchronized by callers if shared across threads.
class PingSession {
public:
    PingSession(TargetConfig target,
                std::shared_ptr<PlatformPingBackend> backend,
                std::shared_ptr<StatisticsAggregator> aggregator);
    virtual ~PingSession() = default;

    PingSession(const PingSession&) = delete;
    PingSession& operator=(const PingSession&) = delete;
    PingSession(PingSession&&) = default;
    PingSession& operator=(PingSession&&) = default;

    // Start periodic pinging for the configured target.
    virtual void start() = 0;

    // Stop the ongoing pinging activity.
    virtual void stop() = 0;

    // Adjust the interval between pings in seconds.
    virtual void set_interval(double seconds) = 0;

    // Access the immutable target configuration.
    [[nodiscard]] virtual const TargetConfig& get_target() const = 0;

protected:
    // Implementations may hold worker thread/task handles; no storage defined here.
    TargetConfig target_;
    std::shared_ptr<PlatformPingBackend> backend_;
    std::shared_ptr<StatisticsAggregator> aggregator_;
};

}  // namespace pingstats

