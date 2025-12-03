project(user)

add_executable(user user.cpp)
target_compile_options(user PRIVATE -Wall -Wextra)