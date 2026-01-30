#pragma once

#include <chrono>
#include <memory>

namespace pingstats {

class StatisticsAggregator;

// Renders statistics to the console.
// Thread-safety: rendering should be coordinated externally if called concurrently.
class ConsoleView {
public:
    explicit ConsoleView(std::shared_ptr<StatisticsAggregator> aggregator);
    virtual ~ConsoleView() = default;

    ConsoleView(const ConsoleView&) = delete;
    ConsoleView& operator=(const ConsoleView&) = delete;
    ConsoleView(ConsoleView&&) = default;
    ConsoleView& operator=(ConsoleView&&) = default;

    // Render the current state once.
    virtual void render_once() = 0;

    // Periodically render statistics until stopped.
    virtual void render_periodic(std::chrono::milliseconds period) = 0;

    // Stop periodic rendering.
    virtual void stop() = 0;

protected:
    std::shared_ptr<StatisticsAggregator> aggregator_;
};

}  // namespace pingstats

