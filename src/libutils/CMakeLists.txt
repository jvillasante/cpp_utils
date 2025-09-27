add_library(libutils
    ${CMAKE_SOURCE_DIR}/include/libutils/algorithms.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/chrono.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/collections.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/functional.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/hash.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/iterators.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/math.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/polyfill.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/print.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/scope_guard.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/smart_pointers.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/strings.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/testing.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/threading.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/unique_handler.hpp
    ${CMAKE_SOURCE_DIR}/include/libutils/unused.hpp
)
add_library(utils::libutils ALIAS libutils)
set_target_properties(libutils PROPERTIES OUTPUT_NAME utils)
set_target_properties(libutils PROPERTIES LINKER_LANGUAGE CXX)

# Since we specify `PUBLIC` visibility here, any targets that compile against
# `libmath` will inherit these properties!
target_compile_features(libutils PUBLIC cxx_std_23)
target_compile_options(libutils PUBLIC
    $<$<CONFIG:Debug>:-Wall -Wextra -Werror -Wpedantic>
)

# For `./src/libmath/include` we use `PRIVATE` visibility as dependent targets
# shouldn't transitively add it to their include directories.
target_include_directories(libutils
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/libutils/include
)

include(GNUInstallDirs)
install(TARGETS libutils
    EXPORT utils-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT utils-targets
    FILE utils-config.cmake
    NAMESPACE utils::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/utils
)
