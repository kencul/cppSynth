cmake_minimum_required(VERSION 3.10)

# Set the project name
project(OscillatorProject)


# Define the path to the header directory
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/Headers")

# Find all .cpp files in the header directory
file(GLOB HEADER_SOURCES "${HEADER_DIR}/*.cpp")

# Create a static library from the .cpp files in the header directory
add_library(header_lib STATIC ${HEADER_SOURCES})

# Add the header directory to the library's include path
# The PUBLIC keyword ensures that targets linking against header_lib also get this include path
target_include_directories(header_lib PUBLIC ${HEADER_DIR})


# Define the path to the oscillator directory
set(OSCILLATOR_DIR "${CMAKE_SOURCE_DIR}/Oscillators")

# Find all .cpp files in the oscillator directory
file(GLOB OSCILLATOR_SOURCES "${OSCILLATOR_DIR}/*.cpp")


foreach(SOURCE_FILE ${OSCILLATOR_SOURCES})
    # Extract the filename without the path
    get_filename_component(EXE_NAME ${SOURCE_FILE} NAME_WE)
    
    # Create an executable for each file
    add_executable(${EXE_NAME} ${SOURCE_FILE})

    # Place all executables directly into bin/
    set_target_properties(${EXE_NAME} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Link with sndfile
    target_link_libraries(${EXE_NAME} sndfile portaudio header_lib)
endforeach()
