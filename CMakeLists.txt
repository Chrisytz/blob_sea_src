cmake_minimum_required(VERSION 3.22)
project(blob_sea_src)

set(CMAKE_CXX_STANDARD 14)

# Including GLFW
add_subdirectory(glfw)
include_directories(glfw/include)

add_executable(blob_sea_src glad.c main.cpp)

# Linking GLFW
target_link_libraries(blob_sea_src glfw)

find_package(OpenGL REQUIRED)

target_link_libraries(blob_sea_src OpenGL::GL)

