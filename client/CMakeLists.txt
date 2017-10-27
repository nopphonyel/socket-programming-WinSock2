cmake_minimum_required(VERSION 3.8)
project(client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
link_libraries(ws2_32 wsock32)
add_executable(client ${SOURCE_FILES})