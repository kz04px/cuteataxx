cmake_minimum_required(VERSION 3.12)

# Project
project(cuteataxx-cli VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/core/)
include_directories(${CMAKE_SOURCE_DIR}/libs/)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Flags
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Add cuteataxx executable
add_executable(
    cuteataxx-cli

    main.cpp

    # CLI
    parse/openings.cpp
    parse/settings.cpp
    match/play.cpp
    match/run.cpp
    match/worker.cpp

    # Core
    ../core/ataxx/parse_move.cpp
)

target_link_libraries(
    cuteataxx-cli
    Threads::Threads
    nlohmann_json::nlohmann_json
    ataxx_static
)
