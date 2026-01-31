#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "console_view.hpp"
#include "statistics_aggregator.hpp"

namespace pingstats {

/// Concrete console renderer that owns a background render thread.
class ConsoleViewImpl : public ConsoleView {
public:
    explicit ConsoleViewImpl(std::shared_ptr<StatisticsAggregator> aggregator);
    ~ConsoleViewImpl() override;

    /// Render a single frame of statistics (header, table, sparkline, histogram).
    void render_once() override;
    /// Start periodic rendering at the given period; idempotent.
    void render_periodic(std::chrono::milliseconds period) override;
    /// Stop periodic rendering and join the worker thread.
    void stop() override;

private:
    /// Clear screen and print heading with current timestamp.
    void render_header() const;
    /// Render KPI table per host (count, loss, min/max/mean/median, last RTT).
    void render_table(const std::vector<StatisticsSnapshot>& snapshots) const;
    /// Render recent RTT sparklines per host.
    void render_time_series(const std::vector<StatisticsSnapshot>& snapshots) const;
    /// Render RTT histogram bars per host.
    void render_histogram(const std::vector<StatisticsSnapshot>& snapshots) const;

    /// Format number or '-' if unavailable.
    static std::string format_double_or_dash(double value, bool has_value);
    /// Format packet loss percentage or '-'.
    static std::string format_loss(double loss_ratio, bool has_value);
    /// Downsample RTTs into a sparkline string.
    static std::string make_sparkline(const std::vector<double>& values, std::size_t width);
    /// Build proportional histogram bar.
    static std::string make_histogram_bar(double value, double max_value, std::size_t width);

    std::atomic<bool> running_{false};
    std::thread render_thread_;
    std::mutex start_stop_mutex_;
};

/// Factory to create a console view bound to the provided aggregator.
std::shared_ptr<ConsoleView> make_console_view(std::shared_ptr<StatisticsAggregator> aggregator);

}  // namespace pingstats

