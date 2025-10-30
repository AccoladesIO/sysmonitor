cmake_minimum_required(VERSION 3.15)
project(SysMonitor VERSION 1.0.1 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_STATIC "Build static binary" OFF)
option(ENABLE_OPTIMIZATION "Enable process optimization features" ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    set(PLATFORM_SOURCES
        src/platform/WindowsPlatform.cpp
    )
    set(PLATFORM_LIBS psapi ws2_32 pdh)
elseif(APPLE)
    set(PLATFORM_SOURCES
        src/platform/MacOSPlatform.cpp
    )
    set(PLATFORM_LIBS)
else()
    set(PLATFORM_SOURCES
        src/platform/LinuxPlatform.cpp
    )
    set(PLATFORM_LIBS pthread)
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/monitor/SystemMonitor.cpp
    src/monitor/ProcessInfo.cpp
    src/visualizer/Visualizer.cpp
    src/optimizer/Optimizer.cpp
    src/utils/Config.cpp
    src/utils/Logger.cpp
    ${PLATFORM_SOURCES}
)

# Create executable
add_executable(sysmonitor ${SOURCES})

# Force console subsystem for Windows/MinGW
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
endif()

# Windows-specific: Set subsystem to console
if(WIN32)
    target_link_options(sysmonitor PRIVATE -mconsole)
endif()

# Link libraries
target_link_libraries(sysmonitor PRIVATE ${PLATFORM_LIBS})

# Compiler warnings
if(MSVC)
    target_compile_options(sysmonitor PRIVATE /W4 /WX-)
else()
    target_compile_options(sysmonitor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Static linking option
if(BUILD_STATIC)
    if(WIN32)
        set_target_properties(sysmonitor PROPERTIES LINK_FLAGS "-static")
    else()
        set_target_properties(sysmonitor PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
    endif()
endif()

# Optimization features
if(ENABLE_OPTIMIZATION)
    target_compile_definitions(sysmonitor PRIVATE ENABLE_OPTIMIZATION=1)
endif()

# Installation
install(TARGETS sysmonitor
    RUNTIME DESTINATION bin
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Package configuration
set(CPACK_PACKAGE_NAME "SysMonitor")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "System Performance Monitor & Optimizer")
set(CPACK_PACKAGE_VENDOR "Your Name")

include(CPack)