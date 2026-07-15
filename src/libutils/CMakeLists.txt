add_library(libutils INTERFACE)
add_library(utils::libutils ALIAS libutils)
set_target_properties(libutils PROPERTIES OUTPUT_NAME utils)
set_target_properties(libutils PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(libutils INTERFACE cxx_std_23)
target_compile_options(libutils INTERFACE
    $<$<CONFIG:Debug>:-Wall -Wextra -Werror -Wpedantic>
)

target_include_directories(libutils
    INTERFACE
          $<INSTALL_INTERFACE:include>
          $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/utils-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/utils-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/utils
)
