cmake_minimum_required(VERSION 3.16)

project(highperfbox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(highperfbox
    src/main.cpp
)

target_include_directories(highperfbox
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)