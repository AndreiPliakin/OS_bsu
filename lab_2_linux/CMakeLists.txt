cmake_minimum_required(VERSION 3.16)
project(lab_2_linux)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(solve solve.cpp)