cmake_minimum_required(VERSION 3.10)
project(TCP_Serialization C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -g)

add_library(binary_tree STATIC binary_tree.c)
add_library(buffer STATIC buffer.c)

target_link_libraries(binary_tree PRIVATE buffer)

add_executable(client tcp_client.c)
target_link_libraries(client PRIVATE binary_tree buffer)

add_executable(server tcp_server.c)
target_link_libraries(server PRIVATE binary_tree buffer)
