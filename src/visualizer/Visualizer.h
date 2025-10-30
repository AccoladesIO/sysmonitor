// ============================================================================
// FILE: src/visualizer/Visualizer.h
// ============================================================================
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "../monitor/ProcessInfo.h"
#include <string>
#include <deque>

class Visualizer {
private:
    static const int GRAPH_WIDTH = 60;
    static const int GRAPH_HEIGHT = 15;
    
    std::string createBar(double percentage, int width = 50);
    std::string createSparkline(const std::deque<double>& data, int width = GRAPH_WIDTH);
    std::string getColorCode(double value);
    
public:
    Visualizer();
    void displayMetrics(const SystemMetrics& metrics, bool show_optimization, 
                       double baseline_cpu, double baseline_mem);
    void clearScreen();
    void showHelpOverlay();
};

#endif // VISUALIZER_H