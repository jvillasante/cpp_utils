add_executable(chrono_example chrono_example.cpp)
target_link_libraries(chrono_example
    PRIVATE utils::libutils)

include(GNUInstallDirs)
install(TARGETS chrono_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
