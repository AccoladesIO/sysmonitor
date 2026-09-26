#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "SysMonitor v2.0.0" << std::endl;
#if defined(_WIN32)
    std::cout << "Platform: Windows" << std::endl;
#elif defined(__APPLE__)
    std::cout << "Platform: macOS" << std::endl;
#elif defined(__linux__)
    std::cout << "Platform: Linux" << std::endl;
#else
    std::cout << "Platform: Unknown" << std::endl;
#endif

    if (argc > 1 && std::string(argv[1]) == "--version") {
        std::cout << "Build: Windows MinGW" << std::endl;
        return 0;
    }

    std::cout << "Usage: sysmonitor.exe [--version]" << std::endl;
    return 0;
}