# TODO – Histogram bucket configuration (compile-time + CLI)
- [ ] Define schema/content for default bucket file and install step (e.g., `config/buckets_default.json`) with hard-fallback when absent.
- [ ] Implement shared loader + validator for bucket boundaries (non-empty, ascending, non-negative, max length) reused by file and CLI inputs, with clear warnings/errors.
- [ ] Wire startup to load compile-time defaults before TargetConfig creation and enforce precedence chain CLI > file > hard default when building aggregator inputs.
- [ ] Add CLI option `--config-file <path>` to override bucket default file location and integrate with loader.
- [ ] Add CLI option `--bucket-boundaries <list|preset>` to parse comma lists/presets, validate, and apply a run-wide override.
- [ ] Ensure TargetConfig propagates final boundaries into session setup and StatisticsAggregator initialization.
- [ ] Tests: unit coverage for validator, file load success/fail, CLI parsing/precedence; integration/smoke to confirm aggregator uses overrides.
- [ ] Docs/help: usage examples for file and CLI options, default path and precedence, sample bucket JSON snippet.
