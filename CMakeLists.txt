cmake_minimum_required(VERSION 3.12)

project(
    CuteAtaxx
    VERSION 1.0
    DESCRIPTION "Running Ataxx engine matches"
    LANGUAGES CXX
)

# Default build type
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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

if(Boost_FOUND AND Threads_FOUND)
    add_subdirectory(src/cli)
else()
    message(WARNING "Can't build cuteataxx-cli: Boost and Threads required")
endif()
