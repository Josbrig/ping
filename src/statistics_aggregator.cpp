#include "statistics_aggregator_impl.hpp"

#include <algorithm>
#include <deque>
#include <limits>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pingstats {

namespace {
constexpr std::size_t kMedianCapacity = 1024;
constexpr std::size_t kRecentCapacity = 256;
const std::vector<double> kDefaultBoundaries{10.0, 20.0, 50.0, 100.0, 200.0, 500.0};

double clamp_non_negative(double value)
{
    return value < 0.0 ? 0.0 : value;
}
}

struct StatisticsAggregatorImpl::Impl {
    struct HostStats {
        std::string host;
        std::vector<double> boundaries;
        std::vector<std::size_t> histogram_counts;
        std::size_t sent_count{0};
        std::size_t success_count{0};
        double min_ms{std::numeric_limits<double>::infinity()};
        double max_ms{-std::numeric_limits<double>::infinity()};
        double sum_ms{0.0};
        std::deque<double> median_buffer;
        std::deque<double> recent_rtts;

        HostStats(std::string h, const std::vector<double>& bounds)
            : host(std::move(h)), boundaries(bounds), histogram_counts(bounds.size() + 1, 0)
        {
        }
    };

    HostStats& ensure_host(const std::string& host)
    {
        auto it = hosts.find(host);
        if (it == hosts.end()) {
            auto [inserted_it, _] = hosts.emplace(host, HostStats{host, kDefaultBoundaries});
            return inserted_it->second;
        }
        return it->second;
    }

    static void reset_stats(HostStats& stats)
    {
        stats.sent_count = 0;
        stats.success_count = 0;
        stats.min_ms = std::numeric_limits<double>::infinity();
        stats.max_ms = -std::numeric_limits<double>::infinity();
        stats.sum_ms = 0.0;
        std::fill(stats.histogram_counts.begin(), stats.histogram_counts.end(), 0);
        stats.median_buffer.clear();
        stats.recent_rtts.clear();
    }

    static double compute_median(const std::deque<double>& buffer)
    {
        if (buffer.empty()) {
            return 0.0;
        }
        std::vector<double> data(buffer.begin(), buffer.end());
        const std::size_t n = data.size();
        const std::size_t mid = n / 2;
        if (n % 2 == 1) {
            std::nth_element(data.begin(), data.begin() + mid, data.end());
            return data[mid];
        }
        std::nth_element(data.begin(), data.begin() + mid - 1, data.end());
        double lower = data[mid - 1];
        std::nth_element(data.begin(), data.begin() + mid, data.end());
        double upper = data[mid];
        return (lower + upper) / 2.0;
    }

    static StatisticsSnapshot build_snapshot(const HostStats& stats)
    {
        StatisticsSnapshot snap;
        snap.host = stats.host;
        snap.count = stats.sent_count;
        if (stats.sent_count == 0) {
            snap.loss_ratio = 0.0;
        } else {
            snap.loss_ratio = static_cast<double>(stats.sent_count - stats.success_count) /
                              static_cast<double>(stats.sent_count);
        }

        if (stats.success_count == 0) {
            snap.min_ms = 0.0;
            snap.max_ms = 0.0;
            snap.mean_ms = 0.0;
            snap.median_ms = 0.0;
        } else {
            snap.min_ms = stats.min_ms;
            snap.max_ms = stats.max_ms;
            snap.mean_ms = stats.sum_ms / static_cast<double>(stats.success_count);
            snap.median_ms = compute_median(stats.median_buffer);
        }

        snap.histogram_buckets.reserve(stats.histogram_counts.size());
        for (std::size_t i = 0; i < stats.histogram_counts.size(); ++i) {
            double boundary = stats.boundaries.empty()
                                  ? 0.0
                                  : (i < stats.boundaries.size() ? stats.boundaries[i]
                                                                 : stats.boundaries.back());
            snap.histogram_buckets.emplace_back(boundary, static_cast<double>(stats.histogram_counts[i]));
        }

        snap.recent_rtts.assign(stats.recent_rtts.begin(), stats.recent_rtts.end());
        return snap;
    }

    mutable std::mutex mutex;
    std::unordered_map<std::string, HostStats> hosts;
};

StatisticsAggregatorImpl::StatisticsAggregatorImpl() : impl_(std::make_unique<Impl>()) {}

void StatisticsAggregatorImpl::add_sample(const std::string& host, double rtt_ms, bool success)
{
    double rtt = clamp_non_negative(rtt_ms);
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto& stats = impl_->ensure_host(host);
    ++stats.sent_count;
    if (!success) {
        return;
    }

    ++stats.success_count;
    stats.sum_ms += rtt;
    stats.min_ms = std::min(stats.min_ms, rtt);
    stats.max_ms = std::max(stats.max_ms, rtt);

    if (stats.median_buffer.size() >= kMedianCapacity) {
        stats.median_buffer.pop_front();
    }
    stats.median_buffer.push_back(rtt);

    std::size_t bucket = stats.boundaries.size();
    for (std::size_t i = 0; i < stats.boundaries.size(); ++i) {
        if (rtt < stats.boundaries[i]) {
            bucket = i;
            break;
        }
    }
    ++stats.histogram_counts[bucket];

    if (stats.recent_rtts.size() >= kRecentCapacity) {
        stats.recent_rtts.pop_front();
    }
    stats.recent_rtts.push_back(rtt);
}

StatisticsSnapshot StatisticsAggregatorImpl::snapshot(const std::string& host) const
{
    std::unique_lock<std::mutex> lock(impl_->mutex);
    auto it = impl_->hosts.find(host);
    if (it == impl_->hosts.end()) {
        return StatisticsSnapshot{};
    }
    auto stats_copy = it->second;
    lock.unlock();
    return Impl::build_snapshot(stats_copy);
}

std::vector<StatisticsSnapshot> StatisticsAggregatorImpl::snapshot_all() const
{
    std::vector<Impl::HostStats> copies;
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        copies.reserve(impl_->hosts.size());
        for (const auto& [_, stats] : impl_->hosts) {
            copies.push_back(stats);
        }
    }

    std::vector<StatisticsSnapshot> result;
    result.reserve(copies.size());
    for (const auto& stats : copies) {
        result.push_back(Impl::build_snapshot(stats));
    }
    return result;
}

void StatisticsAggregatorImpl::reset(const std::string& host)
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    auto it = impl_->hosts.find(host);
    if (it == impl_->hosts.end()) {
        return;
    }
    Impl::reset_stats(it->second);
}

void StatisticsAggregatorImpl::reset_all()
{
    std::lock_guard<std::mutex> lock(impl_->mutex);
    for (auto& [_, stats] : impl_->hosts) {
        Impl::reset_stats(stats);
    }
}

std::shared_ptr<StatisticsAggregator> make_statistics_aggregator()
{
    return std::make_shared<StatisticsAggregatorImpl>();
}

}  // namespace pingstats

