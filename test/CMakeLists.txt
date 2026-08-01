include(${PROJECT_SOURCE_DIR}/cmake/Catch2.cmake)
include(Catch)

# Build the entire test suite across a matrix of C++ standards. The library
# only mandates a C++17 floor (see src/libutils/CMakeLists.txt); consumers pick
# their own standard, so we verify every one we claim to support. Each test
# source becomes one executable per standard, named <test>_cxx<std> (e.g.
# strings_cxx17, strings_cxx20, strings_cxx23).
#
# The polyfill target is special only in that its fallbacks activate below
# C++20; the matrix exercises both paths for free (polyfill_cxx17 uses the
# fallbacks, polyfill_cxx20/23 use the std versions).
set(UTILS_TEST_STANDARDS 17 20 23)

set(UTILS_TESTS
    algorithms
    bit
    chrono
    collections
    functional
    hash
    iterators
    math
    overloaded
    polyfill
    print
    scope_guard
    smart_pointers
    strings
    testing
    threading
    unique_handle)

foreach(test IN LISTS UTILS_TESTS)
    foreach(std IN LISTS UTILS_TEST_STANDARDS)
        set(target ${test}_cxx${std})
        add_executable(${target} ${test}.cpp)
        target_link_libraries(${target}
            PRIVATE
                utils::libutils
                Catch2::Catch2WithMain)
        set_target_properties(${target}
            PROPERTIES
                CXX_STANDARD ${std}
                CXX_STANDARD_REQUIRED ON
                CXX_EXTENSIONS OFF)
        target_compile_options(${target}
            PRIVATE
                -Wno-c2y-extensions)

        # Let Catch2 register each TEST_CASE individually
        catch_discover_tests(${target})
    endforeach()
endforeach()
