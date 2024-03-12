cmake_minimum_required(VERSION 3.12)

# Project
project(tests VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/libs/)

# Flags
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Add cuteataxx executable
add_executable(
    test

    main.cpp

    ../src/core/play.cpp
    ../src/core/ataxx/adjudicate.cpp
    ../src/core/ataxx/parse_move.cpp
    ../src/core/engine/create.cpp

    core/play.cpp
    core/ataxx/adjudicate.cpp
    core/ataxx/parse_move.cpp
    core/tournament/gauntlet.cpp
    core/tournament/roundrobin.cpp
)

target_link_libraries(
    test
    Threads::Threads
    doctest::doctest
    ataxx_static
)
