#include "json_exporter.hpp"

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
    oss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

std::string escape_json(const std::string& s)
{
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
        case '"': oss << "\\\""; break;
        case '\\': oss << "\\\\"; break;
        case '\b': oss << "\\b"; break;
        case '\f': oss << "\\f"; break;
        case '\n': oss << "\\n"; break;
        case '\r': oss << "\\r"; break;
        case '\t': oss << "\\t"; break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                    << static_cast<int>(static_cast<unsigned char>(c));
            } else {
                oss << c;
            }
        }
    }
    return oss.str();
}

}  // namespace

void write_snapshots_json(const std::string& path,
                          const std::vector<StatisticsSnapshot>& snapshots,
                          std::chrono::system_clock::time_point timestamp)
{
    std::ofstream ofs(path, std::ios::trunc);
    if (!ofs.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + path);
    }

    ofs << "{\n";
    ofs << "  \"timestamp\": \"" << escape_json(format_timestamp(timestamp)) << "\",\n";
    ofs << "  \"hosts\": [\n";

    for (std::size_t i = 0; i < snapshots.size(); ++i) {
        const auto& snap = snapshots[i];
        ofs << "    {\n";
        ofs << "      \"host\": \"" << escape_json(snap.host) << "\",\n";
        ofs << "      \"count\": " << snap.count << ",\n";
        ofs << "      \"loss_ratio\": " << snap.loss_ratio << ",\n";
        ofs << "      \"min_ms\": " << snap.min_ms << ",\n";
        ofs << "      \"max_ms\": " << snap.max_ms << ",\n";
        ofs << "      \"mean_ms\": " << snap.mean_ms << ",\n";
        ofs << "      \"median_ms\": " << snap.median_ms << ",\n";

        ofs << "      \"histogram_buckets\": [";
        for (std::size_t b = 0; b < snap.histogram_buckets.size(); ++b) {
            const auto& bucket = snap.histogram_buckets[b];
            ofs << "[" << bucket.first << "," << bucket.second << "]";
            if (b + 1 < snap.histogram_buckets.size()) {
                ofs << ",";
            }
        }
        ofs << "],\n";

        ofs << "      \"recent_rtts\": [";
        for (std::size_t r = 0; r < snap.recent_rtts.size(); ++r) {
            ofs << snap.recent_rtts[r];
            if (r + 1 < snap.recent_rtts.size()) {
                ofs << ",";
            }
        }
        ofs << "]\n";

        ofs << "    }";
        if (i + 1 < snapshots.size()) {
            ofs << ",";
        }
        ofs << "\n";
    }

    ofs << "  ]\n";
    ofs << "}\n";
}

}  // namespace pingstats

