#pragma once

#include <memory>

#include "statistics_aggregator.hpp"

namespace pingstats {

/// Concrete implementation of StatisticsAggregator with internal pImpl storage.
class StatisticsAggregatorImpl : public StatisticsAggregator {
public:
    StatisticsAggregatorImpl();
    ~StatisticsAggregatorImpl() override = default;

    StatisticsAggregatorImpl(const StatisticsAggregatorImpl&) = delete;
    StatisticsAggregatorImpl& operator=(const StatisticsAggregatorImpl&) = delete;
    StatisticsAggregatorImpl(StatisticsAggregatorImpl&&) = delete;
    StatisticsAggregatorImpl& operator=(StatisticsAggregatorImpl&&) = delete;

    /// Record one measurement; success=false counts as loss.
    void add_sample(const std::string& host, double rtt_ms, bool success) override;
    /// Retrieve metrics for a single host (empty snapshot if unknown).
    [[nodiscard]] StatisticsSnapshot snapshot(const std::string& host) const override;
    /// Retrieve metrics for all hosts.
    [[nodiscard]] std::vector<StatisticsSnapshot> snapshot_all() const override;
    /// Reset one host's statistics.
    void reset(const std::string& host) override;
    /// Reset statistics for all hosts.
    void reset_all() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

/// Factory for shared StatisticsAggregator instance hiding concrete type.
std::shared_ptr<StatisticsAggregator> make_statistics_aggregator();

}  // namespace pingstats

