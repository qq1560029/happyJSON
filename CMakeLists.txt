cmake_minimum_required(VERSION 2.6.4)

project( happyJSON )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(googletest-master/googletest)
include_directories(googletest-master/googletest/include)

add_executable(json_test json_test.cpp)

target_link_libraries(json_test gtest gtest_main)