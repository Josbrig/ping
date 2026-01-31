# Report – Effort to switch histogram to non-linear buckets (finer at low RTT, coarser at high RTT)

## Context
- Histogram buckets are currently fixed to linear boundaries {10,20,50,100,200,500} ms in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17) and are adopted per host during creation [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
- Bucket assignment uses simple threshold comparisons [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:148) and rendering uses those boundaries directly [`console_view_impl.cpp`](src/console_view_impl.cpp:213).
- The CLI/config contains `bucket_boundaries`, but they are currently not injected into aggregator instances [`config.hpp`](include/config.hpp:15).

## Options and Effort
1) **Static logarithmic defaults (quick win)**
   - Change default boundaries to a log-like sequence (e.g., 0.5,1,2,5,10,20,50,100,200,500,1000 ms) in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17).
   - Existing assignment/rendering remains unchanged; labels adapt automatically.
   - Effort: ~1–2 h (code + short docs/changelog + smoke test).

2) **Configurable bucket schemes (presets/custom)**
   - Pass user-defined boundaries from CLI/TargetConfig and use them when creating hosts instead of the default [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
   - Add presets ("fine", "log", "coarse") plus a free-list flag; adjust docs/help; add unit tests for mapping/labeling.
   - Effort: ~2–4 h (CLI plumbing, tests, docs).

3) **Adaptive histogram (dynamic)**
   - Derive edges from observations (e.g., log around median/IQR or quantiles), re-bin when edges change, and ensure snapshot consistency.
   - Requires additional state management and thread-safety review.
   - Effort: ~1–2 days (design, implementation, stability tests).

## Recommendation
- For quick value: implement option 1 (log-spaced defaults) and document as the new default.
- For user control: implement option 2 (presets + free list), keeping log-spaced as the default.
- Option 3 only if explicitly requested for adaptive buckets.
