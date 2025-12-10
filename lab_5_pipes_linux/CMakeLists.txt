cmake_minimum_required(VERSION 3.10)
project(PipeChain CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(M M.cpp)
add_executable(A A.cpp)
add_executable(P P.cpp)
add_executable(S S.cpp)
add_executable(main main.cpp)

target_compile_definitions(A PRIVATE STUDENT_NUMBER=${STUDENT_NUMBER})

