#include <cassert>
#include <memory>
#include <numeric>

#include "statistics_aggregator_impl.hpp"

using namespace pingstats;

static void test_empty_snapshot()
{
    auto agg = make_statistics_aggregator();
    auto snap = agg->snapshot("unknown");
    assert(snap.host.empty());
    assert(snap.count == 0);
    assert(snap.loss_ratio == 0.0);
    assert(snap.min_ms == 0.0);
    assert(snap.max_ms == 0.0);
    assert(snap.mean_ms == 0.0);
    assert(snap.median_ms == 0.0);
}

static void test_basic_success_and_loss()
{
    auto agg = make_statistics_aggregator();
    agg->add_sample("host", 10.0, true);
    agg->add_sample("host", 20.0, true);
    agg->add_sample("host", 0.0, false);

    auto snap = agg->snapshot("host");
    assert(snap.count == 3);
    assert(snap.loss_ratio == (3.0 - 2.0) / 3.0);
    assert(snap.min_ms == 10.0);
    assert(snap.max_ms == 20.0);
    assert(snap.mean_ms == 15.0);
    assert(snap.median_ms == 15.0);
    assert(!snap.histogram_buckets.empty());
    assert(!snap.recent_rtts.empty());
}

static void test_histogram_bucket_edges()
{
    auto agg = make_statistics_aggregator();
    // Default boundaries: 10,20,50,100,200,500
    agg->add_sample("h", 5.0, true);    // bucket 0
    agg->add_sample("h", 10.0, true);   // boundary => bucket 1
    agg->add_sample("h", 55.0, true);   // bucket 2 (50-100)
    agg->add_sample("h", 500.0, true);  // bucket last-1? actually boundary match => last bucket index 5
    agg->add_sample("h", 800.0, true);  // last bucket index 6

    auto snap = agg->snapshot("h");
    assert(snap.histogram_buckets.size() == 7);
    assert(snap.histogram_buckets[0].second == 1.0);
    assert(snap.histogram_buckets[1].second == 1.0);
    assert(snap.histogram_buckets[2].second == 0.0);
    assert(snap.histogram_buckets[3].second == 1.0);
    assert(snap.histogram_buckets[4].second == 0.0);
    assert(snap.histogram_buckets[5].second == 1.0);
    assert(snap.histogram_buckets[6].second == 1.0);
}

static void test_reset()
{
    auto agg = make_statistics_aggregator();
    agg->add_sample("r", 30.0, true);
    agg->add_sample("r", 40.0, true);
    agg->reset("r");
    auto snap = agg->snapshot("r");
    assert(snap.count == 0);
    assert(snap.mean_ms == 0.0);
    assert(snap.histogram_buckets.empty() || std::accumulate(
                                                  snap.histogram_buckets.begin(),
                                                  snap.histogram_buckets.end(),
                                                  0.0,
                                                  [](double acc, const auto& p) { return acc + p.second; }) == 0.0);
}

int main()
{
    test_empty_snapshot();
    test_basic_success_and_loss();
    test_histogram_bucket_edges();
    test_reset();
    return 0;
}

