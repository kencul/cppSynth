cmake_minimum_required(VERSION 3.10)

# Find all .cpp files in this directory
file(GLOB EXAMPLE_SOURCES *.cpp)

foreach(SOURCE_FILE ${EXAMPLE_SOURCES})
    # Extract the filename without the path
    get_filename_component(EXE_NAME ${SOURCE_FILE} NAME_WE)
    
    # Create an executable for each file
    add_executable(${EXE_NAME} ${SOURCE_FILE})

    # Place all executables directly into bin/
    set_target_properties(${EXE_NAME} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    # Link with sndfile
    target_link_libraries(${EXE_NAME} sndfile)
endforeach()
