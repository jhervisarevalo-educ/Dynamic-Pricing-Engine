# ---------------------------------------------------------------------------
# Dynamic Pricing Engine using an epsilon-Greedy Multi-Armed Bandit
# ---------------------------------------------------------------------------
# Minimal, portable CMake configuration. No external dependencies are used:
# the project relies solely on the C++17 standard library (in particular the
# <random> header for pseudo-random number generation).
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14)

project(DynamicPricingEngine
    VERSION 1.0.0
    DESCRIPTION "Dynamic pricing engine using an epsilon-Greedy Multi-Armed Bandit"
    LANGUAGES CXX)

# Enforce the C++17 standard for all targets.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to a Release build when none is specified.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# The console executable.
add_executable(pricing_engine
    src/main.cpp
    src/EpsilonGreedyBandit.cpp
    src/MarketSimulator.cpp
    src/PricingEngine.cpp)

# Public headers live under include/.
target_include_directories(pricing_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable a healthy set of warnings to keep the code clean.
if(MSVC)
    target_compile_options(pricing_engine PRIVATE /W4)
else()
    target_compile_options(pricing_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
