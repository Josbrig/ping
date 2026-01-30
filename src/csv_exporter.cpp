#include "csv_exporter.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace pingstats {

namespace {

std::string format_timestamp(std::chrono::system_clock::time_point tp)
{
    using clock = std::chrono::system_clock;
    const std::time_t t = clock::to_time_t(tp);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

}  // namespace

void write_snapshots_csv_append(const std::string& path,
                                const std::vector<StatisticsSnapshot>& snapshots,
                                std::chrono::system_clock::time_point timestamp)
{
    if (snapshots.empty()) {
        return;
    }

    const bool file_exists = std::filesystem::exists(path);
    std::ofstream ofs(path, std::ios::app);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + path);
    }

    if (!file_exists) {
        ofs << "timestamp,host,count,loss_ratio,min_ms,max_ms,mean_ms,median_ms\n";
    }

    const std::string ts = format_timestamp(timestamp);
    for (const auto& snap : snapshots) {
        ofs << '"' << ts << '"'
            << ',' << '"' << snap.host << '"'
            << ',' << snap.count
            << ',' << snap.loss_ratio
            << ',' << snap.min_ms
            << ',' << snap.max_ms
            << ',' << snap.mean_ms
            << ',' << snap.median_ms
            << '\n';
    }
}

}  // namespace pingstats

