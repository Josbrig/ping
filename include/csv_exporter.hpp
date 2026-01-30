#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "statistics_aggregator.hpp"

namespace pingstats {

// Append snapshots to a CSV file. If the file is new/empty, a header is written.
// Throws std::runtime_error on I/O errors.
void write_snapshots_csv_append(const std::string& path,
                                const std::vector<StatisticsSnapshot>& snapshots,
                                std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());

}  // namespace pingstats

