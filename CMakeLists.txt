cmake_minimum_required(VERSION 3.23)
project(tape_sort)

set(CMAKE_CXX_STANDARD 20)

add_executable(tape_sort src/main.cpp include/tape.h include/sort.h src/sort.cpp)
