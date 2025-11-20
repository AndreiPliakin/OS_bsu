cmake_minimum_required(VERSION 3.10)
project(BufferedChannelTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(buffered_channel
        test_buffered_channel.cpp
)

target_compile_features(buffered_channel PRIVATE cxx_std_17)
target_compile_options(buffered_channel_test PRIVATE -Wall -Wextra -pthread)