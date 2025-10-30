#include "Visualizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <chrono>

Visualizer::Visualizer() {}

void Visualizer::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

std::string Visualizer::getColorCode(double value) {
    if (value > 80) return "\033[41m"; // Red background
    if (value > 60) return "\033[43m"; // Yellow background
    return "\033[42m"; // Green background
}

std::string Visualizer::createBar(double percentage, int width) {
    int filled = static_cast<int>((percentage / 100.0) * width);
    std::string bar = "[";
    
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            bar += getColorCode(percentage) + " \033[0m";
        } else {
            bar += "\033[47m \033[0m"; // Gray
        }
    }
    
    bar += "]";
    return bar;
}

std::string Visualizer::createSparkline(const std::deque<double>& data, int width) {
    if (data.empty()) return std::string(width, ' ');
    
    const char* bars[] = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
    double max_val = *std::max_element(data.begin(), data.end());
    if (max_val == 0) max_val = 1.0;
    
    std::string sparkline;
    int data_points = std::min(width, static_cast<int>(data.size()));
    int start_idx = data.size() - data_points;
    
    for (size_t i = start_idx; i < data.size(); i++) {
        double normalized = data[i] / max_val;
        int bar_idx = static_cast<int>(normalized * 7);
        bar_idx = std::min(7, std::max(0, bar_idx));
        
        if (data[i] > 80) sparkline += "\033[31m";
        else if (data[i] > 60) sparkline += "\033[33m";
        else sparkline += "\033[32m";
        
        sparkline += bars[bar_idx];
        sparkline += "\033[0m";
    }
    
    return sparkline;
}

void Visualizer::displayMetrics(const SystemMetrics& metrics, bool show_optimization,
                                double baseline_cpu, double baseline_mem) {
    clearScreen();
    
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "\033[1;36m╔════════════════════════════════════════════════════════════════════════╗\033[0m\n";
    std::cout << "\033[1;36m║          SYSTEM PERFORMANCE MONITOR & OPTIMIZER v2.0                   ║\033[0m\n";
    std::cout << "\033[1;36m╚════════════════════════════════════════════════════════════════════════╝\033[0m\n";
    
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now_c));
    std::cout << "Time: " << time_str << "\n\n";
    
    // CPU Section
    std::cout << "\033[1;33m┌─ CPU USAGE ────────────────────────────────────────────────────────────┐\033[0m\n";
    std::cout << "│ Current: " << std::fixed << std::setprecision(2) << metrics.cpu_usage << "%  ";
    
    if (baseline_cpu > 0) {
        double improvement = ((baseline_cpu - metrics.cpu_usage) / baseline_cpu) * 100;
        if (improvement > 0) {
            std::cout << "(\033[32m↓ " << improvement << "%\033[0m improvement)";
        } else {
            std::cout << "(\033[31m↑ " << -improvement << "%\033[0m from baseline)";
        }
    }
    std::cout << "\n│\n";
    std::cout << "│ " << createBar(metrics.cpu_usage, 60) << " " 
              << std::fixed << std::setprecision(1) << metrics.cpu_usage << "%\n";
    std::cout << "\033[1;33m└────────────────────────────────────────────────────────────────────────┘\033[0m\n\n";
    
    // Memory Section
    std::cout << "\033[1;35m┌─ MEMORY USAGE ─────────────────────────────────────────────────────────┐\033[0m\n";
    std::cout << "│ Total: " << metrics.total_mem_kb / 1024 << " MB  |  "
              << "Used: " << metrics.used_mem_kb / 1024 << " MB  |  "
              << "Available: " << metrics.available_mem_kb / 1024 << " MB\n";
    std::cout << "│\n";
    std::cout << "│ " << createBar(metrics.mem_usage_percent, 60) << " "
              << std::fixed << std::setprecision(1) << metrics.mem_usage_percent << "%\n";
    std::cout << "\033[1;35m└────────────────────────────────────────────────────────────────────────┘\033[0m\n\n";
    
    // Top Processes
    std::cout << "\033[1;32m┌─ TOP PROCESSES (by CPU) ───────────────────────────────────────────────┐\033[0m\n";
    std::cout << "│ " << std::left << std::setw(8) << "PID"
              << std::setw(20) << "Name"
              << std::setw(12) << "CPU %"
              << std::setw(14) << "Memory (MB)"
              << std::setw(10) << "Priority" << "│\n";
    std::cout << "│ " << std::string(64, '─') << "│\n";
    
    for (const auto& proc : metrics.top_processes) {
        std::cout << "│ " << std::left << std::setw(8) << proc.pid
                  << std::setw(20) << proc.name.substr(0, 19)
                  << std::setw(12) << std::fixed << std::setprecision(1) << proc.cpu_usage
                  << std::setw(14) << proc.memory_kb / 1024
                  << std::setw(10) << proc.priority << "│\n";
    }
    std::cout << "\033[1;32m└────────────────────────────────────────────────────────────────────────┘\033[0m\n\n";
    
    if (show_optimization) {
        std::cout << "\033[1;31m┌─ OPTIMIZATION STATUS ──────────────────────────────────────────────────┐\033[0m\n";
        bool optimized = false;
        for (const auto& proc : metrics.top_processes) {
            if (proc.cpu_usage > 80.0) {
                std::cout << "│ ⚡ Optimizing: " << std::left << std::setw(20) << proc.name
                          << " (PID: " << proc.pid << ")" << std::string(25, ' ') << "│\n";
                optimized = true;
            }
        }
        if (!optimized) {
            std::cout << "│ ✓ System performing optimally" << std::string(43, ' ') << "│\n";
        }
        std::cout << "\033[1;31m└────────────────────────────────────────────────────────────────────────┘\033[0m\n";
    }
    
    std::cout << "\n\033[90mPress 'q' to exit  |  'h' for help  |  'o' to toggle optimization\033[0m\n";
}

void Visualizer::showHelpOverlay() {
    std::cout << "\n\033[1;36m╔════════════════════════════════════════════════════╗\033[0m\n";
    std::cout << "\033[1;36m║              KEYBOARD SHORTCUTS                    ║\033[0m\n";
    std::cout << "\033[1;36m╠════════════════════════════════════════════════════╣\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  q / Ctrl+C  -  Exit program                      \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  o           -  Toggle optimization               \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  r           -  Reset baseline metrics            \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  p           -  Pause/Resume monitoring           \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  +/-         -  Adjust update interval            \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  h           -  Show this help                    \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m║\033[0m  s           -  Save snapshot                     \033[1;36m║\033[0m\n";
    std::cout << "\033[1;36m╚════════════════════════════════════════════════════╝\033[0m\n\n";
}