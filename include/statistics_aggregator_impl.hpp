#pragma once

#include <memory>

#include "statistics_aggregator.hpp"

namespace pingstats {

class StatisticsAggregatorImpl : public StatisticsAggregator {
public:
    StatisticsAggregatorImpl();
    ~StatisticsAggregatorImpl() override = default;

    StatisticsAggregatorImpl(const StatisticsAggregatorImpl&) = delete;
    StatisticsAggregatorImpl& operator=(const StatisticsAggregatorImpl&) = delete;
    StatisticsAggregatorImpl(StatisticsAggregatorImpl&&) = delete;
    StatisticsAggregatorImpl& operator=(StatisticsAggregatorImpl&&) = delete;

    void add_sample(const std::string& host, double rtt_ms, bool success) override;
    [[nodiscard]] StatisticsSnapshot snapshot(const std::string& host) const override;
    [[nodiscard]] std::vector<StatisticsSnapshot> snapshot_all() const override;
    void reset(const std::string& host) override;
    void reset_all() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

std::shared_ptr<StatisticsAggregator> make_statistics_aggregator();

}  // namespace pingstats

