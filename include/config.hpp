#pragma once

#include <optional>
#include <string>
#include <vector>

namespace pingstats {

// Output format selection for exporting aggregated statistics.
// Not thread-safe for concurrent mutation; synchronize externally.
enum class OutputFormat { None, Csv, Json };

// Target configuration container for a single host.
// Thread-safety: callers must coordinate concurrent access.
struct TargetConfig {
    std::string host;
    std::optional<double> interval_s;
    std::optional<OutputFormat> output_format;
    std::optional<std::string> output_file;
    std::vector<double> bucket_boundaries;

    TargetConfig() = default;
    ~TargetConfig() = default;
    TargetConfig(const TargetConfig&) = default;
    TargetConfig& operator=(const TargetConfig&) = default;
    TargetConfig(TargetConfig&&) = default;
    TargetConfig& operator=(TargetConfig&&) = default;
};

}  // namespace pingstats
