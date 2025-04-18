cmake_minimum_required(VERSION 3.10)
project(rbtree_lab C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -g")

# src 디렉토리 포함
add_subdirectory(src)

# test 디렉토리 포함
add_subdirectory(test)

