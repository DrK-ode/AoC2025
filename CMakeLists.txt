cmake_minimum_required(VERSION 3.21)
project(
  AoC2025
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "-std=c++26 -O3 -march=native -pedantic -pedantic-errors -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Werror -Wno-unused")
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-15")

add_executable(run01 day01.cpp)
add_executable(run02 day02.cpp)
add_executable(run03 day03.cpp)
add_executable(run04 day04.cpp)
add_executable(run05 day05.cpp)