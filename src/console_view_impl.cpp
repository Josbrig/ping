#include "console_view_impl.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace pingstats {

namespace {
constexpr const char* CLEAR_SCREEN = "\x1b[2J\x1b[H";
constexpr std::size_t TABLE_HOST_WIDTH = 20;
constexpr std::size_t TABLE_NUMBER_WIDTH = 8;
constexpr std::size_t SPARKLINE_WIDTH = 40;
constexpr std::size_t HISTOGRAM_BAR_WIDTH = 30;
constexpr std::size_t LOSS_PERCENT_PRECISION = 1;
constexpr std::size_t MS_PRECISION = 1;
constexpr const char* SPARK_CHARS = " .:-=+*#%@";  // 10 levels low->high

std::string format_time_now()
{
    using clock = std::chrono::system_clock;
    const auto now = clock::now();
    const std::time_t t = clock::to_time_t(now);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

template <typename T>
std::string pad_right(const T& value, std::size_t width)
{
    std::ostringstream oss;
    oss << std::left << std::setw(static_cast<int>(width)) << value;
    return oss.str();
}

std::string format_ms(double value)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(MS_PRECISION) << value;
    return oss.str();
}

}  // namespace

ConsoleView::ConsoleView(std::shared_ptr<StatisticsAggregator> aggregator)
    : aggregator_(std::move(aggregator))
{
}

ConsoleViewImpl::ConsoleViewImpl(std::shared_ptr<StatisticsAggregator> aggregator)
    : ConsoleView(std::move(aggregator))
{
}

ConsoleViewImpl::~ConsoleViewImpl()
{
    stop();
}

void ConsoleViewImpl::render_header() const
{
    std::cout << CLEAR_SCREEN;
    std::cout << "Ping Statistics - " << format_time_now() << "\n";
    std::cout << std::string(80, '=') << "\n";
}

std::string ConsoleViewImpl::format_double_or_dash(double value, bool has_value)
{
    if (!has_value || std::isnan(value) || std::isinf(value)) {
        return "-";
    }
    return format_ms(value);
}

std::string ConsoleViewImpl::format_loss(double loss_ratio, bool has_value)
{
    if (!has_value || loss_ratio < 0.0) {
        return "-";
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(LOSS_PERCENT_PRECISION) << (loss_ratio * 100.0);
    return oss.str();
}

std::string ConsoleViewImpl::make_sparkline(const std::vector<double>& values, std::size_t width)
{
    if (values.empty() || width == 0) {
        return "";
    }

    std::vector<double> samples;
    if (values.size() <= width) {
        samples = values;
    } else {
        samples.reserve(width);
        const double step = static_cast<double>(values.size()) / static_cast<double>(width);
        for (std::size_t i = 0; i < width; ++i) {
            const auto idx = static_cast<std::size_t>(std::floor(i * step));
            samples.push_back(values[idx]);
        }
    }

    const auto [min_it, max_it] = std::minmax_element(samples.begin(), samples.end());
    const double min_v = *min_it;
    const double max_v = *max_it;

    std::string line;
    line.reserve(samples.size());
    const std::size_t levels = 10;
    for (double v : samples) {
        std::size_t level = 0;
        if (max_v > min_v) {
            const double norm = (v - min_v) / (max_v - min_v);
            level = static_cast<std::size_t>(std::round(norm * (levels - 1)));
        } else {
            level = levels / 2;  // flat line
        }
        level = std::min<std::size_t>(level, levels - 1);
        line.push_back(SPARK_CHARS[level]);
    }
    return line;
}

std::string ConsoleViewImpl::make_histogram_bar(double value, double max_value, std::size_t width)
{
    if (max_value <= 0.0) {
        return {};
    }
    const double ratio = value / max_value;
    const auto filled = static_cast<std::size_t>(std::round(ratio * static_cast<double>(width)));
    return std::string(filled, '#');
}

void ConsoleViewImpl::render_table(const std::vector<StatisticsSnapshot>& snapshots) const
{
    const std::string header =
        pad_right("HOST", TABLE_HOST_WIDTH) +
        pad_right("N", TABLE_NUMBER_WIDTH) +
        pad_right("LOSS%", TABLE_NUMBER_WIDTH) +
        pad_right("MIN", TABLE_NUMBER_WIDTH) +
        pad_right("MAX", TABLE_NUMBER_WIDTH) +
        pad_right("MEAN", TABLE_NUMBER_WIDTH) +
        pad_right("MED", TABLE_NUMBER_WIDTH) +
        pad_right("LAST", TABLE_NUMBER_WIDTH);
    std::cout << header << "\n";
    std::cout << std::string(header.size(), '-') << "\n";

    for (const auto& snap : snapshots) {
        const bool has_data = snap.count > 0;
        const std::string loss = format_loss(snap.loss_ratio, has_data);
        const std::string min = format_double_or_dash(snap.min_ms, has_data);
        const std::string max = format_double_or_dash(snap.max_ms, has_data);
        const std::string mean = format_double_or_dash(snap.mean_ms, has_data);
        const std::string med = format_double_or_dash(snap.median_ms, has_data);
        std::string last = "-";
        if (!snap.recent_rtts.empty()) {
            last = format_ms(snap.recent_rtts.back());
        }

        std::cout
            << pad_right(snap.host, TABLE_HOST_WIDTH)
            << pad_right(snap.count, TABLE_NUMBER_WIDTH)
            << pad_right(loss, TABLE_NUMBER_WIDTH)
            << pad_right(min, TABLE_NUMBER_WIDTH)
            << pad_right(max, TABLE_NUMBER_WIDTH)
            << pad_right(mean, TABLE_NUMBER_WIDTH)
            << pad_right(med, TABLE_NUMBER_WIDTH)
            << pad_right(last, TABLE_NUMBER_WIDTH)
            << "\n";
    }

    std::cout << "\n";
}

void ConsoleViewImpl::render_time_series(const std::vector<StatisticsSnapshot>& snapshots) const
{
    std::cout << "Recent RTTs (sparkline)" << "\n";
    std::cout << std::string(80, '-') << "\n";
    for (const auto& snap : snapshots) {
        std::cout << pad_right(snap.host, TABLE_HOST_WIDTH);
        if (snap.recent_rtts.empty()) {
            std::cout << "(no data)";
        } else {
            std::cout << make_sparkline(snap.recent_rtts, SPARKLINE_WIDTH);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void ConsoleViewImpl::render_histogram(const std::vector<StatisticsSnapshot>& snapshots) const
{
    std::cout << "Histogram" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& snap : snapshots) {
        std::cout << snap.host << "\n";
        if (snap.histogram_buckets.empty()) {
            std::cout << "  (no histogram data)\n";
            continue;
        }

        double max_count = 0.0;
        for (const auto& bucket : snap.histogram_buckets) {
            max_count = std::max(max_count, bucket.second);
        }

        for (std::size_t i = 0; i < snap.histogram_buckets.size(); ++i) {
            const auto& bucket = snap.histogram_buckets[i];
            std::string label;
            if (i == 0) {
                label = "<= " + format_ms(bucket.first);
            } else {
                const double prev = snap.histogram_buckets[i - 1].first;
                label = format_ms(prev) + " - " + format_ms(bucket.first);
            }
            if (i == snap.histogram_buckets.size() - 1) {
                label = ">= " + format_ms(snap.histogram_buckets.back().first);
            }

            const std::string bar = make_histogram_bar(bucket.second, max_count, HISTOGRAM_BAR_WIDTH);
            std::cout << "  " << pad_right(label, 16) << " " << pad_right(bar, HISTOGRAM_BAR_WIDTH)
                      << " " << bucket.second << "\n";
        }
    }

    std::cout << "\n";
}

void ConsoleViewImpl::render_once()
{
    auto snapshots = aggregator_->snapshot_all();
    std::sort(snapshots.begin(), snapshots.end(), [](const auto& a, const auto& b) {
        return a.host < b.host;
    });

    render_header();
    render_table(snapshots);
    render_time_series(snapshots);
    render_histogram(snapshots);

    std::cout.flush();
}

void ConsoleViewImpl::render_periodic(std::chrono::milliseconds period)
{
    std::lock_guard<std::mutex> lock(start_stop_mutex_);
    if (running_) {
        return;
    }
    running_ = true;
    render_thread_ = std::thread([this, period]() {
        while (running_) {
            render_once();
            std::this_thread::sleep_for(period);
        }
    });
}

void ConsoleViewImpl::stop()
{
    std::lock_guard<std::mutex> lock(start_stop_mutex_);
    if (!running_) {
        return;
    }
    running_ = false;
    if (render_thread_.joinable()) {
        render_thread_.join();
    }
}

std::shared_ptr<ConsoleView> make_console_view(std::shared_ptr<StatisticsAggregator> aggregator)
{
    return std::make_shared<ConsoleViewImpl>(std::move(aggregator));
}

}  // namespace pingstats

