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
    GIT_TAG 2fe5d1517c9e66fefb78cef7b3c35e5f9f1bb9ee
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

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG cdf28903a7781f97ba94d30b79c3a4b0c97ccce7
)

FetchContent_MakeAvailable(libataxx json doctest ftxui)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(Boost_FOUND AND Threads_FOUND)
    add_subdirectory(src/cli)
    add_subdirectory(src/tui)
    add_subdirectory(tests)
else()
    message(WARNING "Can't build cuteataxx-cli: Boost and Threads required")
endif()
