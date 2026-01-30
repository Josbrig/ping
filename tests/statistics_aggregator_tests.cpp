#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "statistics_aggregator_impl.hpp"

using namespace pingstats;
using Catch::Approx;

TEST_CASE("empty snapshot has zeroed metrics")
{
    auto agg = make_statistics_aggregator();
    auto snap = agg->snapshot("unknown");
    REQUIRE(snap.host.empty());
    REQUIRE(snap.count == 0);
    REQUIRE(snap.loss_ratio == 0.0);
    REQUIRE(snap.min_ms == 0.0);
    REQUIRE(snap.max_ms == 0.0);
    REQUIRE(snap.mean_ms == 0.0);
    REQUIRE(snap.median_ms == 0.0);
}

TEST_CASE("basic success and loss updates stats")
{
    auto agg = make_statistics_aggregator();
    agg->add_sample("host", 10.0, true);
    agg->add_sample("host", 20.0, true);
    agg->add_sample("host", 0.0, false);

    auto snap = agg->snapshot("host");
    REQUIRE(snap.count == 3);
    REQUIRE(snap.loss_ratio == Approx((3.0 - 2.0) / 3.0));
    REQUIRE(snap.min_ms == Approx(10.0));
    REQUIRE(snap.max_ms == Approx(20.0));
    REQUIRE(snap.mean_ms == Approx(15.0));
    REQUIRE(snap.median_ms == Approx(15.0));
    REQUIRE_FALSE(snap.histogram_buckets.empty());
    REQUIRE_FALSE(snap.recent_rtts.empty());
}

TEST_CASE("histogram bucket edges distribute counts")
{
    auto agg = make_statistics_aggregator();
    // Default boundaries: 10,20,50,100,200,500
    agg->add_sample("h", 5.0, true);    // bucket 0
    agg->add_sample("h", 10.0, true);   // boundary => bucket 1
    agg->add_sample("h", 55.0, true);   // bucket 3 (50-100)
    agg->add_sample("h", 500.0, true);  // boundary => last bucket
    agg->add_sample("h", 800.0, true);  // last bucket overflow

    auto snap = agg->snapshot("h");
    REQUIRE(snap.histogram_buckets.size() == 7);
    REQUIRE(snap.histogram_buckets[0].second == Approx(1.0));
    REQUIRE(snap.histogram_buckets[1].second == Approx(1.0));
    REQUIRE(snap.histogram_buckets[2].second == Approx(0.0));
    REQUIRE(snap.histogram_buckets[3].second == Approx(1.0));
    REQUIRE(snap.histogram_buckets[4].second == Approx(0.0));
    REQUIRE(snap.histogram_buckets[5].second == Approx(0.0));
    REQUIRE(snap.histogram_buckets[6].second == Approx(2.0));
}

TEST_CASE("reset clears stats")
{
    auto agg = make_statistics_aggregator();
    agg->add_sample("r", 30.0, true);
    agg->add_sample("r", 40.0, true);
    agg->reset("r");
    auto snap = agg->snapshot("r");
    REQUIRE(snap.count == 0);
    double sum = 0.0;
    for (const auto& p : snap.histogram_buckets) {
        sum += p.second;
    }
    REQUIRE(sum == Approx(0.0));
}

