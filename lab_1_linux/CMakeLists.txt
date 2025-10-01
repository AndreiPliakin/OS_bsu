cmake_minimum_required(VERSION 3.10)
project(MyProject CXX)

add_library(Number STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Number/Number.cpp)
target_include_directories(Number PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Number)
set_target_properties(Number PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Vector SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/Vector/Vector.cpp)
target_include_directories(Vector PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Vector
    ${CMAKE_CURRENT_SOURCE_DIR}/Number)
target_link_libraries(Vector PRIVATE Number)
target_compile_definitions(Vector PRIVATE VECTORLIB_EXPORTS)

add_executable(Main
    ${CMAKE_CURRENT_SOURCE_DIR}/Main/main.cpp)
target_include_directories(Main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Number
    ${CMAKE_CURRENT_SOURCE_DIR}/Vector)
target_link_libraries(Main PRIVATE Number Vector)

set_target_properties(Number PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(Vector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(Main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)