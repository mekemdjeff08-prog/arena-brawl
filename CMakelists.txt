cmake_minimum_required(VERSION 3.10)
project(ArenaBrawl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(arena-brawl
    src/main.cpp
    src/AI.cpp
    src/Competence.cpp
    src/Equipement.cpp
    src/Game.cpp
    src/Utils.cpp
)

target_include_directories(arena-brawl PRIVATE include)

if(MSVC)
    target_compile_options(arena-brawl PRIVATE /W4)
else()
    target_compile_options(arena-brawl PRIVATE -Wall -Wextra)
endif()