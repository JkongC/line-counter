cmake_minimum_required(VERSION 4.0)
project(line-counter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/src)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
elseif (NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Release"))
    message(WARNING "\"${CMAKE_BUILD_TYPE}\" is not accepted for build type. Will use Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

add_executable(lcount "src/main.cpp")

if (MSVC)
    target_compile_options(lcount PRIVATE /W4)
else()
    target_compile_options(lcount PRIVATE -Wall -Wextra -Wpedantic)
endif()