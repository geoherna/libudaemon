cmake_minimum_required(VERSION 2.8.0)
project(lud_test)

include_directories(../src/include)



file(GLOB TEST_SOURCES "./test.c")

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable(lud_test ${TEST_SOURCES})
target_link_libraries(lud_test udaemon)
