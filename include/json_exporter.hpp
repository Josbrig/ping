#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "statistics_aggregator.hpp"

namespace pingstats {

// Write snapshots to a JSON file (overwrites existing file).
// Throws std::runtime_error on I/O errors.
void write_snapshots_json(const std::string& path,
                          const std::vector<StatisticsSnapshot>& snapshots,
                          std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());

}  // namespace pingstats

