@echo off
echo Building SysMonitor...

g++ -std=c++11 -O3 ^
    -Iinclude -Isrc ^
    src/main.cpp ^
    src/monitor/SystemMonitor.cpp ^
    src/monitor/ProcessInfo.cpp ^
    src/visualizer/Visualizer.cpp ^
    src/optimizer/Optimizer.cpp ^
    src/utils/Config.cpp ^
    src/utils/Logger.cpp ^
    src/platform/WindowsPlatform.cpp ^
    -lpsapi -lpdh ^
    -Wl,--subsystem,console ^
    -o sysmonitor.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo BUILD SUCCESSFUL!
    echo Run: sysmonitor.exe start
) else (
    echo.
    echo Build failed
    pause
)
