cmake_minimum_required(VERSION 3.10)
project(osc_examples)

set(CMAKE_CXX_STANDARD 17)

# Define output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add include directories
include_directories(/opt/homebrew/include)

# Add link directories
link_directories(/opt/homebrew/lib)

# Include the Oscillators directory
add_subdirectory(Oscillators)
