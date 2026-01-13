cmake_minimum_required(VERSION 3.10)
project(killer)

add_executable(killer killer_windows.cpp
        ../user/user_windows.cpp
        ../../lab_5_pipes_windows/main.cpp)