@echo off
echo ╔════════════════════════════════════════════════════════╗
echo ║  SysMonitor Installation Script (Windows)             ║
echo ╚════════════════════════════════════════════════════════╝
echo.

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake not found. Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Check for Visual Studio or MinGW
where cl >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set GENERATOR="Visual Studio 16 2019"
    goto :build
)

where g++ >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set GENERATOR="MinGW Makefiles"
    goto :build
)

echo ❌ No C++ compiler found. Please install Visual Studio or MinGW-w64
pause
exit /b 1

:build
echo ✓ Prerequisites check passed
echo.

echo Building SysMonitor...
if not exist build mkdir build
cd build

cmake .. -G %GENERATOR% -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed
    pause
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed
    pause
    exit /b 1
)

echo ✓ Build successful
echo.

echo Installing to C:\Program Files\SysMonitor...
cmake --install . --prefix "C:\Program Files\SysMonitor"

echo.
echo ✓ Installation successful
echo.
echo Add to PATH: C:\Program Files\SysMonitor\bin
echo Then run: sysmonitor start
pause