cmake_minimum_required(VERSION 3.12)

project(
    CuteAtaxx
    VERSION 1.0
    DESCRIPTION "Running Ataxx engine matches"
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(
    libataxx
    GIT_REPOSITORY https://github.com/kz04px/libataxx
    GIT_TAG 4226c26dd11a1f74be708882ece6fd9dc96c767b
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest
    GIT_TAG v2.4.10
)

FetchContent_MakeAvailable(libataxx json doctest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(Boost_FOUND AND Threads_FOUND)
    add_subdirectory(src/cli)
    add_subdirectory(tests)
else()
    message(WARNING "Can't build cuteataxx-cli: Boost and Threads required")
endif()
