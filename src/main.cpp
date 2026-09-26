#include "sysmonitor/Version.h"
#include "monitor/SystemMonitor.h"
#include "visualizer/Visualizer.h"
#include "optimizer/Optimizer.h"
#include "utils/Config.h"
#include "utils/Logger.h"
#include "platform/Platform.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        running = false;
    }
}

void showHelp(const char* program) {
    std::cout << "\n";
    std::cout << "SysMonitor v" << SYSMONITOR_VERSION << " - System Performance Monitor\n\n";
    std::cout << "USAGE:\n";
    std::cout << "  " << program << " <command> [options]\n\n";
    std::cout << "COMMANDS:\n";
    std::cout << "  start              Start monitoring\n";
    std::cout << "  --help, -h         Show this help\n";
    std::cout << "  --version, -v      Show version\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  -o, --optimize              Enable auto-optimization\n";
    std::cout << "  -i, --interval <seconds>    Update interval (default: 2)\n";
    std::cout << "  -t, --threshold <percent>   CPU threshold (default: 80)\n";
    std::cout << "  -q, --quiet                 Minimal output\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << program << " start\n";
    std::cout << "  " << program << " start -o -i 5\n";
    std::cout << "  " << program << " start --optimize --interval 3\n\n";
}

void showVersion() {
    std::cout << "SysMonitor version " << SYSMONITOR_VERSION << "\n";
#if defined(_WIN32)
    std::cout << "Platform: Windows\n";
#elif defined(__APPLE__)
    std::cout << "Platform: macOS\n";
#elif defined(__linux__)
    std::cout << "Platform: Linux\n";
#else
    std::cout << "Platform: Unknown\n";
#endif
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::string command = "start";

    Platform::ensureDirectoryExists(Platform::getConfigDirectory());

    Config config;
    config.load();

    int interval = config.interval;
    bool auto_optimize = config.optimize;
    int threshold = config.threshold;
    bool quiet = false;

    if (argc > 1) {
        command = argv[1];

        if (command == "--help" || command == "-h") {
            showHelp(argv[0]);
            return 0;
        }

        if (command == "--version" || command == "-v") {
            showVersion();
            return 0;
        }

        if (command == "start") {
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];

                if (arg == "-o" || arg == "--optimize") {
                    auto_optimize = true;
                }
                else if (arg == "-i" || arg == "--interval") {
                    if (i + 1 < argc) {
                        try {
                            interval = std::stoi(argv[++i]);
                        } catch (const std::exception&) {
                            std::cerr << "Invalid value for --interval: " << argv[i] << "\n";
                            return 1;
                        }
                    }
                }
                else if (arg == "-t" || arg == "--threshold") {
                    if (i + 1 < argc) {
                        try {
                            threshold = std::stoi(argv[++i]);
                        } catch (const std::exception&) {
                            std::cerr << "Invalid value for --threshold: " << argv[i] << "\n";
                            return 1;
                        }
                    }
                }
                else if (arg == "-q" || arg == "--quiet") {
                    quiet = true;
                }
            }
        }
    }

    if (command == "start") {
        try {
            config.interval = interval;
            config.optimize = auto_optimize;
            config.threshold = threshold;

            Logger logger(Platform::getConfigDirectory() + "/sysmonitor.log");
            SystemMonitor monitor;
            Visualizer visualizer;
            Optimizer optimizer(threshold);

            if (!quiet) {
                std::cout << "\nSysMonitor v" << SYSMONITOR_VERSION << " Starting...\n\n";
                std::cout << "Configuration:\n";
                std::cout << "  Update Interval: " << interval << " seconds\n";
                std::cout << "  Auto-Optimize: " << (auto_optimize ? "Enabled" : "Disabled") << "\n";
                std::cout << "  CPU Threshold: " << threshold << "%\n\n";
            }

            logger.log("Establishing baseline metrics...");
            monitor.establishBaseline();

            if (auto_optimize && !quiet) {
                std::cout << "Auto-optimization: ENABLED\n";
                std::cout << "Note: Run as Administrator for best results\n\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));

            while (running) {
                auto metrics = monitor.collectMetrics();

                if (!quiet) {
                    visualizer.displayMetrics(metrics, auto_optimize,
                                             monitor.getBaselineCPU(),
                                             monitor.getBaselineMem());
                }

                if (auto_optimize) {
                    auto optimized = optimizer.optimizeProcesses(metrics.top_processes);
                    for (const auto& proc : optimized) {
                        logger.log("Optimized process: " + proc.name +
                                  " (PID: " + std::to_string(proc.pid) + ")");
                    }
                }

                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }

            std::cout << "\n\nShutting down gracefully...\n";
            logger.log("Monitoring stopped");

            if (config.auto_save) {
                if (!config.save()) {
                    logger.log("Failed to save configuration", "WARN");
                }
            }

            std::cout << "Monitoring stopped successfully\n";

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
    } else {
        std::cerr << "Unknown command: " << command << "\n";
        std::cerr << "Use --help for usage information\n";
        return 1;
    }

    return 0;
}