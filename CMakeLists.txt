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
    GIT_TAG 5240a0ae30144b7df6c1bdcfa1a37466806bbedd
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_MakeAvailable(libataxx json)

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(Boost_FOUND AND Threads_FOUND)
    add_subdirectory(src/cli)
else()
    message(WARNING "Can't build cuteataxx-cli: Boost and Threads required")
endif()
