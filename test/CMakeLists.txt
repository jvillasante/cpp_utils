include(${PROJECT_SOURCE_DIR}/cmake/Catch2.cmake)

add_executable(algorithms algorithms.cpp)
target_link_libraries(algorithms
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(algorithms ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/algorithms)

add_executable(bit bit.cpp)
target_link_libraries(bit
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(bit ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bit)

add_executable(collections collections.cpp)
target_link_libraries(collections
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(collections ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/collections)

add_executable(functional functional.cpp)
target_link_libraries(functional
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(functional ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/functional)

add_executable(hash hash.cpp)
target_link_libraries(hash
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(hash ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hash)

add_executable(math math.cpp)
target_link_libraries(math
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(math ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/math)

add_executable(scope_guard scope_guard.cpp)
target_link_libraries(scope_guard
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(scope_guard ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scope_guard)

add_executable(smart_pointers smart_pointers.cpp)
target_link_libraries(smart_pointers
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(smart_pointers ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/smart_pointers)

add_executable(strings strings.cpp)
target_link_libraries(strings
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(strings ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/strings)

add_executable(testing testing.cpp)
target_link_libraries(testing
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(testing ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testing)

add_executable(print print.cpp)
target_link_libraries(print
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(print ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/print)

add_executable(unique_handle unique_handle.cpp)
target_link_libraries(unique_handle
    PRIVATE utils::libutils Catch2::Catch2WithMain)
add_test(unique_handle ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unique_handle)
