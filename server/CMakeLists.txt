cmake_minimum_required(VERSION 3.8)
project(socket_project)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES server.cpp)
link_libraries(ws2_32 wsock32 libgcc_s_seh-1 libwinpthread-1)
add_executable(socket_project ${SOURCE_FILES})