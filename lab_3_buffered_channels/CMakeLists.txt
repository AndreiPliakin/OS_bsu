cmake_minimum_required(VERSION 3.10)
project(BufferedChannelTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включить все предупреждения
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Для pthread на Linux
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Основной исполняемый файл с тестом умножения матриц
add_executable(buffered_channel_test
        test_buffered_channel.cpp
        buffered_channel.h  # Добавляем header файл для ясности
)

target_compile_features(buffered_channel_test PRIVATE cxx_std_17)

# Убедитесь что buffered_channel.h находится в той же папке
target_include_directories(buffered_channel_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)