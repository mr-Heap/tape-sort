cmake_minimum_required(VERSION 3.23)
project(tape_sort)

set(CMAKE_CXX_STANDARD 20)

#add_executable(tape_sort src/main.cpp include/tape.h include/sort.h)

enable_testing()

add_subdirectory(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(tape_sort src/main.cpp include/tape.h include/sort.h tests/tests.cpp include/config.h)

target_link_libraries(tape_sort GTest::gtest GTest::gtest_main)

add_test(NAME tests COMMAND tape_sort)