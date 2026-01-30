#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "config.hpp"
#include "console_view_impl.hpp"
#include "csv_exporter.hpp"
#include "ping_session.hpp"
#include "platform_ping_backend_factory.hpp"
#include "statistics_aggregator_impl.hpp"

namespace pingstats {

namespace {

constexpr double kDefaultIntervalSeconds = 1.0;
constexpr auto kDefaultRenderPeriod = std::chrono::milliseconds{500};
constexpr std::string_view kVersion = "0.1.0";
constexpr auto kDefaultExportPeriod = std::chrono::seconds{5};

struct CliOptions {
    std::optional<double> interval_s;
    std::optional<OutputFormat> output_format;
    std::optional<std::string> output_file;
    std::vector<std::string> hosts;
    bool show_help{false};
    bool show_version{false};
};

[[noreturn]] void throw_cli_error(const std::string& msg) {
    throw std::runtime_error(msg);
}

void print_usage(std::ostream& os, const char* prog_name) {
    os << "Usage: " << prog_name << " [options] <host1> [host2 ...]\n"
       << "Options:\n"
       << "  -h, --help               Show this help and exit\n"
       << "  --version                Show version and exit\n"
       << "  -i, --interval <sec>     Ping interval in seconds (default: "
       << kDefaultIntervalSeconds << ")\n"
       << "  --output-format <fmt>    Output format for export: none|csv|json\n"
       << "  --output-file <path>     Path to export aggregated statistics\n";
}

OutputFormat parse_output_format(const std::string& value) {
    if (value == "none") {
        return OutputFormat::None;
    }
    if (value == "csv") {
        return OutputFormat::Csv;
    }
    if (value == "json") {
        return OutputFormat::Json;
    }
    throw_cli_error("Unknown output format: " + value);
}

CliOptions parse_arguments(int argc, char** argv) {
    CliOptions opts;
    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};
        if (arg == "-h" || arg == "--help") {
            opts.show_help = true;
            return opts;
        }
        if (arg == "--version") {
            opts.show_version = true;
            return opts;
        }
        if (arg == "-i" || arg == "--interval") {
            if (i + 1 >= argc) {
                throw_cli_error("Missing value for interval");
            }
            const std::string val{argv[++i]};
            try {
                opts.interval_s = std::stod(val);
            } catch (const std::exception&) {
                throw_cli_error("Invalid interval value: " + val);
            }
            if (opts.interval_s.value() <= 0.0) {
                throw_cli_error("Interval must be > 0");
            }
            continue;
        }
        if (arg == "--output-format") {
            if (i + 1 >= argc) {
                throw_cli_error("Missing value for output-format");
            }
            const std::string val{argv[++i]};
            opts.output_format = parse_output_format(val);
            continue;
        }
        if (arg == "--output-file") {
            if (i + 1 >= argc) {
                throw_cli_error("Missing value for output-file");
            }
            opts.output_file = std::string{argv[++i]};
            continue;
        }

        // Positional argument = host
        if (!arg.empty() && arg.front() == '-') {
            throw_cli_error("Unknown option: " + arg);
        }
        opts.hosts.push_back(arg);
    }
    return opts;
}

struct SessionBundle {
    std::shared_ptr<PlatformPingBackend> backend;
    std::unique_ptr<PingSession> session;
};

class CsvExporterLoop {
public:
    CsvExporterLoop(std::shared_ptr<StatisticsAggregator> aggregator,
                    std::string path,
                    std::chrono::milliseconds period)
        : aggregator_(std::move(aggregator)), path_(std::move(path)), period_(period)
    {
    }

    void start()
    {
        if (running_) {
            return;
        }
        running_ = true;
        worker_ = std::thread([this]() {
            while (running_) {
                auto snaps = aggregator_->snapshot_all();
                try {
                    write_snapshots_csv_append(path_, snaps);
                } catch (const std::exception& ex) {
                    std::cerr << "CSV export error: " << ex.what() << std::endl;
                }
                std::this_thread::sleep_for(period_);
            }
        });
    }

    void stop()
    {
        running_ = false;
        if (worker_.joinable()) {
            worker_.join();
        }
    }

    ~CsvExporterLoop() { stop(); }

private:
    std::shared_ptr<StatisticsAggregator> aggregator_;
    std::string path_;
    std::chrono::milliseconds period_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

}  // namespace

int run(int argc, char** argv) {
    try {
        auto opts = parse_arguments(argc, argv);

        if (opts.show_help) {
            print_usage(std::cout, argv[0]);
            return 0;
        }
        if (opts.show_version) {
            std::cout << "pingstats version " << kVersion << "\n";
            return 0;
        }

        if (opts.hosts.empty()) {
            print_usage(std::cerr, argv[0]);
            return 2;
        }

        // Resolve defaults
        const double interval = opts.interval_s.value_or(kDefaultIntervalSeconds);
        std::optional<OutputFormat> effective_format = opts.output_format;
        if (!effective_format && opts.output_file) {
            effective_format = OutputFormat::Csv;
        }

        // Build target configs
        std::vector<TargetConfig> targets;
        targets.reserve(opts.hosts.size());
        for (const auto& host : opts.hosts) {
            TargetConfig cfg;
            cfg.host = host;
            cfg.interval_s = interval;
            if (effective_format) {
                cfg.output_format = effective_format;
            }
            if (opts.output_file) {
                cfg.output_file = opts.output_file;
            }
            targets.push_back(std::move(cfg));
        }

        auto aggregator = make_statistics_aggregator();

        CsvExporterLoop csv_loop{aggregator,
                                 opts.output_file.value_or("pingstats.csv"),
                                 kDefaultExportPeriod};
        const bool enable_export = effective_format == OutputFormat::Csv;

        std::vector<SessionBundle> sessions;
        sessions.reserve(targets.size());
        for (auto& target : targets) {
            auto backend_unique = make_platform_ping_backend();
            backend_unique->initialize();
            auto backend_shared = std::shared_ptr<PlatformPingBackend>(std::move(backend_unique));
            auto session = make_ping_session(std::move(target), backend_shared, aggregator);
            sessions.push_back(SessionBundle{std::move(backend_shared), std::move(session)});
        }

        for (auto& bundle : sessions) {
            bundle.session->start();
        }

        if (enable_export) {
            csv_loop.start();
        }

        auto view = make_console_view(aggregator);
        view->render_periodic(kDefaultRenderPeriod);

        std::cout << "Running. Press Enter to stop..." << std::endl;
        std::string line;
        std::getline(std::cin, line);

        view->stop();
        if (enable_export) {
            csv_loop.stop();
            // Final snapshot write
            write_snapshots_csv_append(opts.output_file.value_or("pingstats.csv"),
                                       aggregator->snapshot_all());
        }
        for (auto& bundle : sessions) {
            bundle.session->stop();
            if (bundle.backend) {
                bundle.backend->shutdown();
            }
        }

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
}

}  // namespace pingstats

int main(int argc, char** argv) {
    return pingstats::run(argc, argv);
}
