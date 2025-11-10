cmake_minimum_required(VERSION 3.10)
project(UDP_Datagram C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -g)

add_library(datagram STATIC datagram.c)

add_executable(server udp_server.c)
target_link_libraries(server PRIVATE datagram)

add_executable(client udp_client.c)
target_link_libraries(client PRIVATE datagram)


target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR})
