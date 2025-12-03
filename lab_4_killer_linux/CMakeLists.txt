cmake_minimum_required(VERSION 3.10)
project(lab4_killer_linux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем подпроекты
add_subdirectory(killer)
add_subdirectory(user)