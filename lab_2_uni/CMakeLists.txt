cmake_minimum_required(VERSION 3.16)
project(UniversalMatrixMult)

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_executable(UniversalMatrixMult universal_matrix.cpp)