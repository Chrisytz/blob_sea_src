cmake_minimum_required(VERSION 3.17)
project(blob_sea_src)

set(CMAKE_CXX_STANDARD 14)

# Including GLFW
add_subdirectory(glfw)
include_directories(glfw/include Header_Files Resources Source_Files)

add_executable(blob_sea_src glad.c main.cpp Header_Files/shader.h Source_Files/shader.cpp Header_Files/stb_image.h Source_Files/stb_image.cpp Header_Files/camera.h Source_Files/camera.cpp Header_Files/mesh.h Source_Files/mesh.cpp Header_Files/model.h Source_Files/model.cpp)

# Linking GLFW
target_link_libraries(blob_sea_src glfw)

find_package(OpenGL REQUIRED)

target_link_libraries(blob_sea_src OpenGL::GL)