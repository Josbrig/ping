# TODO – Option 1: Logarithmic histogram defaults

- [ ] Set default boundaries to a log-spaced scale (e.g., 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000) in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17)
- [ ] Smoke test: short run with sample hosts; verify histogram labels
- [ ] Docs/Changelog: add a note about non-linear defaults (README/agent_rules if needed)
