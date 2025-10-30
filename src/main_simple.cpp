#include <iostream>
#include <string>

// Temporary minimal main for testing
int main(int argc, char* argv[]) {
    std::cout << "SysMonitor v2.0.0" << std::endl;
    std::cout << "Platform: Windows" << std::endl;
    
    if (argc > 1 && std::string(argv[1]) == "--version") {
        std::cout << "Build: Windows MinGW" << std::endl;
        return 0;
    }
    
    std::cout << "Usage: sysmonitor.exe [--version]" << std::endl;
    return 0;
}
