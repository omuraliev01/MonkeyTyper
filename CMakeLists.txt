cmake_minimum_required(VERSION 3.24)
project(MonkeyTyper CXX)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)

set(BUILD_SHARED_LIBS FALSE)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.0.2
)
FetchContent_MakeAvailable(SFML fmt)


add_executable(MonkeyTyper
        main.cpp
        src/Config.cpp
        src/Words.cpp
        src/Main.hpp
        src/Config.hpp
        src/Words.hpp
        src/AlignCenter.cpp
        src/AlignCenter.hpp
)

target_link_libraries(MonkeyTyper PRIVATE SFML::Graphics fmt::fmt)
