project(killer)

add_executable(killer killer.cpp)
target_compile_options(killer PRIVATE -Wall -Wextra -pthread)