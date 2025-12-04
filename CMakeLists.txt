cmake_minimum_required(VERSION 3.21)
project(
  AoC2025
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_FLAGS "-std=c++26 -O3 -march=native -Wall -Wextra -Wunused -pedantic-errors")
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(run01 day01.cpp)
add_executable(run02 day02.cpp)