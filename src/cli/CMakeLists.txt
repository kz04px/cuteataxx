cmake_minimum_required(VERSION 3.12)

# Project
project(cuteataxx-cli VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/libs/)
include_directories(${CMAKE_SOURCE_DIR}/libs/libataxx/src/)

# Add cuteataxx executable
add_executable(
    cuteataxx-cli

    main.cpp
    parse/openings.cpp
    parse/settings.cpp
    match/play.cpp
    match/run.cpp
    match/worker.cpp
)

target_link_libraries(
    cuteataxx-cli
    Threads::Threads
    "${CMAKE_SOURCE_DIR}/libs/libataxx/build/static/libataxx.a"
)
