cmake_minimum_required(VERSION 3.16)

project(highperfbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

add_executable(highperfbox
    src/main.cpp
    src/core/app.cpp
    src/log/logger.cpp
    src/log/console_sink.cpp
    src/config/config.cpp
)

configure_file(
    ${PROJECT_SOURCE_DIR}/config.ini
    ${CMAKE_CURRENT_BINARY_DIR}/config.ini
    COPYONLY
)

target_include_directories(highperfbox
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(highperfbox PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

#LRU basic test

add_executable(test_lru_basic
    tests/demo/test_lru_basic.cpp
)

target_include_directories(test_lru_basic
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(test_lru_basic PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

#add_executable(test_workload_basic
#    tests/demo/test_workload_basic.cpp
#)

#target_include_directories(test_workload_basic
#    PUBLIC ${PROJECT_SOURCE_DIR}/include
#)

#target_compile_options(test_workload_basic PRIVATE
#    -Wall
#    -Wextra
#    -Wpedantic
#)

#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    target_compile_options(test_workload_basic PRIVATE -O3)
#endif()


add_executable(test_lru_workload
    tests/demo/test_lru_workload.cpp
)

target_include_directories(test_lru_workload
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(test_lru_workload PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(test_workload tests/demo/test_workload.cpp)

target_include_directories(test_workload PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(test_lru_workload PRIVATE -O3)
endif()

enable_testing()

add_subdirectory(third_party/benchmark)

add_executable(bench_lru_workload benchmark/bench_lru_workload.cpp)
target_link_libraries(bench_lru_workload PRIVATE benchmark::benchmark)
target_include_directories(bench_lru_workload PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

#add_executable(bench_vector benchmark/bm_vector.cpp)
#target_link_libraries(bench_vector benchmark::benchmark)

#add_executable(bench_map_insert benchmark/bm_map_insert.cpp)
#target_link_libraries(bench_map_insert benchmark::benchmark)

#add_executable(bench_unordered_map_reserve benchmark/bm_unordered_map_reserve.cpp)
#target_link_libraries(bench_unordered_map_reserve benchmark::benchmark)

#add_executable(bench_map_find benchmark/bm_map_find.cpp)
#target_link_libraries(bench_map_find benchmark::benchmark)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(highperfbox PRIVATE -O3)
endif()