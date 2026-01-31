#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace pingstats {

/// Immutable snapshot of aggregated statistics for a host.
/// Thread-safety: treated as read-only value type.
class StatisticsSnapshot {
public:
    std::string host;
    std::size_t count{};
    double loss_ratio{};
    double min_ms{};
    double max_ms{};
    double mean_ms{};
    double median_ms{};
    std::vector<std::pair<double, double>> histogram_buckets;  // boundary,value
    std::vector<double> recent_rtts;

    StatisticsSnapshot() = default;
    ~StatisticsSnapshot() = default;
    StatisticsSnapshot(const StatisticsSnapshot&) = default;
    StatisticsSnapshot& operator=(const StatisticsSnapshot&) = default;
    StatisticsSnapshot(StatisticsSnapshot&&) = default;
    StatisticsSnapshot& operator=(StatisticsSnapshot&&) = default;
};

/// Aggregates RTT samples per host.
/// Thread-safety: implementers should make public methods safe for concurrent access.
class StatisticsAggregator {
public:
    virtual ~StatisticsAggregator() = default;

    StatisticsAggregator() = default;
    StatisticsAggregator(const StatisticsAggregator&) = delete;
    StatisticsAggregator& operator=(const StatisticsAggregator&) = delete;
    StatisticsAggregator(StatisticsAggregator&&) = default;
    StatisticsAggregator& operator=(StatisticsAggregator&&) = default;

    /// Record a single measurement result for a host.
    virtual void add_sample(const std::string& host, double rtt_ms, bool success) = 0;

    /// Retrieve a snapshot for a specific host.
    [[nodiscard]] virtual StatisticsSnapshot snapshot(const std::string& host) const = 0;

    /// Retrieve snapshots for all known hosts.
    [[nodiscard]] virtual std::vector<StatisticsSnapshot> snapshot_all() const = 0;

    /// Reset statistics for a specific host.
    virtual void reset(const std::string& host) = 0;

    /// Reset all aggregated statistics.
    virtual void reset_all() = 0;
};

}  // namespace pingstats

