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

class ConsoleViewImpl : public ConsoleView {
public:
    explicit ConsoleViewImpl(std::shared_ptr<StatisticsAggregator> aggregator);
    ~ConsoleViewImpl() override;

    void render_once() override;
    void render_periodic(std::chrono::milliseconds period) override;
    void stop() override;

private:
    void render_header() const;
    void render_table(const std::vector<StatisticsSnapshot>& snapshots) const;
    void render_time_series(const std::vector<StatisticsSnapshot>& snapshots) const;
    void render_histogram(const std::vector<StatisticsSnapshot>& snapshots) const;

    static std::string format_double_or_dash(double value, bool has_value);
    static std::string format_loss(double loss_ratio, bool has_value);
    static std::string make_sparkline(const std::vector<double>& values, std::size_t width);
    static std::string make_histogram_bar(double value, double max_value, std::size_t width);

    std::atomic<bool> running_{false};
    std::thread render_thread_;
    std::mutex start_stop_mutex_;
};

std::shared_ptr<ConsoleView> make_console_view(std::shared_ptr<StatisticsAggregator> aggregator);

}  // namespace pingstats

