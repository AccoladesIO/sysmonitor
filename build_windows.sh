#!/bin/bash
# ============================================================================
# FILE: build_windows.sh
# Windows Build Script for MinGW/Git Bash
# ============================================================================

echo "╔════════════════════════════════════════════════════════╗"
echo "║  SysMonitor Windows Build Script (MinGW)               ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}❌ Error: CMakeLists.txt not found!${NC}"
    echo "Please run this script from the sysmonitor root directory"
    exit 1
fi

echo "📍 Current directory: $(pwd)"
echo ""

# Check for g++
echo "🔍 Checking for g++ (MinGW)..."
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}❌ g++ not found!${NC}"
    echo ""
    echo "Please install MinGW-w64:"
    echo "  Option 1: choco install mingw"
    echo "  Option 2: Download from https://www.mingw-w64.org/downloads/"
    exit 1
fi

GCC_VERSION=$(g++ --version | head -n1)
echo -e "${GREEN}✓ Found: $GCC_VERSION${NC}"
echo ""

# Check for CMake
echo "🔍 Checking for CMake..."
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}❌ CMake not found!${NC}"
    echo ""
    echo "Please install CMake:"
    echo "  Option 1: choco install cmake"
    echo "  Option 2: Download from https://cmake.org/download/"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1)
echo -e "${GREEN}✓ Found: $CMAKE_VERSION${NC}"
echo ""

# Clean build directory
echo "🧹 Cleaning build directory..."
if [ -d "build" ]; then
    rm -rf build
    echo -e "${GREEN}✓ Removed old build directory${NC}"
fi

mkdir build
cd build
echo -e "${GREEN}✓ Created fresh build directory${NC}"
echo ""

# Configure
echo "⚙️  Configuring with CMake (MinGW Makefiles)..."
cmake .. -G "MinGW Makefiles" \
    -DCMAKE_C_COMPILER=gcc \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ CMake configuration failed!${NC}"
    echo ""
    echo "Troubleshooting tips:"
    echo "  1. Ensure MinGW is in your PATH"
    echo "  2. Try running: export PATH=/mingw64/bin:\$PATH"
    echo "  3. Check CMakeCache.txt for errors"
    exit 1
fi

echo -e "${GREEN}✓ Configuration successful${NC}"
echo ""

# Build
echo "🔨 Building SysMonitor..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Build failed!${NC}"
    echo ""
    echo "Check the error messages above for details"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Build successful!${NC}"
echo ""

# Test
echo "🧪 Testing executable..."
if [ -f "bin/sysmonitor.exe" ]; then
    echo -e "${GREEN}✓ Found: bin/sysmonitor.exe${NC}"
    echo ""
    
    # Get file size
    SIZE=$(du -h bin/sysmonitor.exe | cut -f1)
    echo "📦 Size: $SIZE"
    echo ""
    
    # Test version
    echo "🎯 Testing version command..."
    ./bin/sysmonitor.exe --version
    
    if [ $? -eq 0 ]; then
        echo ""
        echo -e "${GREEN}╔════════════════════════════════════════════════════════╗${NC}"
        echo -e "${GREEN}║          🎉 BUILD SUCCESSFUL! 🎉                       ║${NC}"
        echo -e "${GREEN}╚════════════════════════════════════════════════════════╝${NC}"
        echo ""
        echo "To run SysMonitor:"
        echo "  cd $(pwd)"
        echo "  ./bin/sysmonitor.exe start"
        echo ""
        echo "Or with optimization (requires Administrator):"
        echo "  ./bin/sysmonitor.exe start -o"
        echo ""
        echo "To install system-wide:"
        echo "  cp bin/sysmonitor.exe /c/Windows/System32/"
        echo "  (then run from anywhere: sysmonitor start)"
        echo ""
    else
        echo -e "${YELLOW}⚠  Warning: Executable exists but version test failed${NC}"
    fi
else
    echo -e "${RED}❌ Error: sysmonitor.exe not found in bin/${NC}"
    echo "Expected: $(pwd)/bin/sysmonitor.exe"
    ls -la bin/ 2>/dev/null || echo "bin/ directory doesn't exist"
    exit 1
fi