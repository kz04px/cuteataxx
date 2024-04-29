cmake_minimum_required(VERSION 3.12)

# Project
project(cuteataxx-tui VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/libs/)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Flags
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Add cuteataxx executable
add_executable(
    cuteataxx-tui

    main.cpp

    # Core
    ../core/ataxx/adjudicate.cpp
    ../core/ataxx/parse_move.cpp
    ../core/engine/create.cpp
    ../core/play.cpp
    ../core/pgn.cpp
    ../core/parse/openings.cpp
    ../core/parse/settings.cpp
    ../core/match/run.cpp
    ../core/match/worker.cpp
)

target_link_libraries(
    cuteataxx-tui
    PRIVATE ataxx_static
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component

    # Threads::Threads
    nlohmann_json::nlohmann_json
    # ataxx_static
)
