cmake_minimum_required(VERSION 3.15)
project(PipeChainWindows CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

add_executable(M M.cpp)
add_executable(A A.cpp)
add_executable(P P.cpp)
add_executable(S S.cpp)
add_executable(main main.cpp)

if(WIN32)
    target_link_libraries(main Kernel32.lib)
endif()
