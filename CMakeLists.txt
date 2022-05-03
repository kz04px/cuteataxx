cmake_minimum_required(VERSION 3.12)

# Project
project(cuteataxx VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/libataxx/src/)

find_package(Threads REQUIRED)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add cuteataxx executable
add_executable(
    cuteataxx
    src/main.cpp
    src/match/play.cpp
    src/match/run.cpp
    src/match/worker.cpp
    src/parse/openings.cpp
    src/parse/json/engines.cpp
    src/parse/json/file.cpp
    src/parse/json/pgn.cpp
    src/parse/json/timecontrol.cpp
)

target_link_libraries(
    cuteataxx
    Threads::Threads
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/libataxx/build/static/libataxx.a"
)
