##
# Top-level CMake configuration for pingstats.
# - Defines the main executable target and includes subdirectories.
# - Enables testing and optional Doxygen documentation.
##
cmake_minimum_required(VERSION 3.20)

project(pingstats VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

## Main executable; sources added in src/CMakeLists.txt
add_executable(pingstats)

target_include_directories(pingstats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

## Doxygen target (optional if doxygen is found)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

## Convenience target to run all tests with ctest
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests via ctest"
)
