add_executable(algorithms algorithms.cpp)
target_link_libraries(algorithms
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(algorithms ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/algorithms)

add_executable(collections collections.cpp)
target_link_libraries(collections
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(collections ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/collections)

add_executable(unique_handle unique_handle.cpp)
target_link_libraries(unique_handle
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(unique_handle ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unique_handle)
