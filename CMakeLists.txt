cmake_minimum_required(VERSION 3.27)

# Initialize the project and specify languages
project(
  cpp_utils
  VERSION 0.0.1
  DESCRIPTION ""
  LANGUAGES CXX
)

# Setup ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    message(STATUS "Ccache found: ${CCACHE_PROGRAM}")
else()
    message(STATUS "Ccache not found")
endif()

# CTest automatically calls enable_testing() and provides BUILD_TESTING
include(CTest)

# Sources
add_subdirectory("src")
add_subdirectory("tools")

# Conditionally include the test directory
if(BUILD_TESTING)
    add_subdirectory("test")
endif()
