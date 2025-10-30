@echo off
REM ============================================================================
REM FILE: build_windows.bat
REM Windows Build Script for Command Prompt
REM ============================================================================

echo ╔════════════════════════════════════════════════════════╗
echo ║  SysMonitor Windows Build Script                      ║
echo ╚════════════════════════════════════════════════════════╝
echo.

REM Check if CMakeLists.txt exists
if not exist "CMakeLists.txt" (
    echo ❌ Error: CMakeLists.txt not found!
    echo Please run this script from the sysmonitor root directory
    pause
    exit /b 1
)

echo 📍 Current directory: %CD%
echo.

REM Check for CMake
echo 🔍 Checking for CMake...
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake not found!
    echo.
    echo Please install CMake:
    echo   Option 1: choco install cmake
    echo   Option 2: Download from https://cmake.org/download/
    pause
    exit /b 1
)

for /f "tokens=*" %%i in ('cmake --version') do (
    echo ✓ Found: %%i
    goto :cmake_found
)
:cmake_found
echo.

REM Detect build system
echo 🔍 Detecting build system...

REM Check for MinGW
where g++ >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set BUILD_SYSTEM=MinGW
    set GENERATOR=MinGW Makefiles
    echo ✓ Found: MinGW (g++)
    goto :build_system_found
)

REM Check for Visual Studio 2022
where cl >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set BUILD_SYSTEM=MSVC
    set GENERATOR=Visual Studio 17 2022
    echo ✓ Found: Visual Studio 2022
    goto :build_system_found
)

REM Check for Visual Studio 2019
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019" (
    set BUILD_SYSTEM=MSVC
    set GENERATOR=Visual Studio 16 2019
    echo ✓ Found: Visual Studio 2019
    goto :build_system_found
)

REM No build system found
echo ❌ No build system found!
echo.
echo Please install one of:
echo   1. MinGW: choco install mingw
echo   2. Visual Studio: https://visualstudio.microsoft.com/downloads/
pause
exit /b 1

:build_system_found
echo Build System: %BUILD_SYSTEM%
echo Generator: %GENERATOR%
echo.

REM Clean build directory
echo 🧹 Cleaning build directory...
if exist "build" (
    rmdir /s /q build
    echo ✓ Removed old build directory
)

mkdir build
cd build
echo ✓ Created fresh build directory
echo.

REM Configure
echo ⚙️  Configuring with CMake...

if "%BUILD_SYSTEM%"=="MinGW" (
    cmake .. -G "%GENERATOR%" -DCMAKE_BUILD_TYPE=Release
) else (
    cmake .. -G "%GENERATOR%" -A x64
)

if %ERRORLEVEL% NEQ 0 (
    echo ❌ CMake configuration failed!
    echo.
    echo Check the error messages above
    cd ..
    pause
    exit /b 1
)

echo ✓ Configuration successful
echo.

REM Build
echo 🔨 Building SysMonitor...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Build failed!
    echo.
    echo Check the error messages above
    cd ..
    pause
    exit /b 1
)

echo.
echo ✓ Build successful!
echo.

REM Test
echo 🧪 Testing executable...

if "%BUILD_SYSTEM%"=="MinGW" (
    set EXE_PATH=bin\sysmonitor.exe
) else (
    set EXE_PATH=bin\Release\sysmonitor.exe
)

if exist "%EXE_PATH%" (
    echo ✓ Found: %EXE_PATH%
    echo.
    
    REM Test version
    echo 🎯 Testing version command...
    "%EXE_PATH%" --version
    
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo ╔════════════════════════════════════════════════════════╗
        echo ║          🎉 BUILD SUCCESSFUL! 🎉                       ║
        echo ╚════════════════════════════════════════════════════════╝
        echo.
        echo To run SysMonitor:
        echo   cd %CD%
        echo   %EXE_PATH% start
        echo.
        echo Or with optimization (requires Administrator):
        echo   %EXE_PATH% start -o
        echo.
        echo To install system-wide (requires Administrator):
        echo   copy %EXE_PATH% C:\Windows\System32\
        echo   (then run from anywhere: sysmonitor start^)
        echo.
    )
) else (
    echo ❌ Error: sysmonitor.exe not found!
    echo Expected: %CD%\%EXE_PATH%
    dir bin /s
)

cd ..
pause