cmake_minimum_required(VERSION 3.16)

project(highperfbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(highperfbox PRIVATE -O3)
endif()