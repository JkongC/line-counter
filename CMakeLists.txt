cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(line-counter)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
endif()

add_executable(lcount src/main.cpp
    src/Argument.h
    src/Argument.cpp
    src/utils/Array.hpp
    src/log/Logger.hpp
)

target_include_directories(lcount PUBLIC "${PROJECT_SOURCE_DIR}/src")

if(MSVC)
    target_compile_options(lcount PRIVATE /W4)
else()
    target_compile_options(lcount PRIVATE -Wall -Wextra -Wpedantic)
endif()
