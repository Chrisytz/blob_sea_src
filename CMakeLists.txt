cmake_minimum_required(VERSION 3.17)
project(blob_sea_src)

set(CMAKE_CXX_STANDARD 14)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(C:/Users/chris/Downloads/glfw-3.3.7/glfw glfw)
add_subdirectory(C:/Users/chris/Downloads/assimp-5.2.3/assimp assimp)

include_directories(C:/Users/chris/OpenGL_Libraries/Include Header_Files Resources Source_Files)
link_directories(C:/Users/chris/OpenGL_Libraries/Libs)

add_executable(blob_sea_src glad.c main.cpp Header_Files/shader.h Source_Files/shader.cpp Header_Files/stb_image.h Source_Files/stb_image.cpp Header_Files/camera.h Source_Files/camera.cpp Header_Files/mesh.h Source_Files/mesh.cpp Header_Files/model.h Source_Files/model.cpp)

# Linking GLFW
target_link_libraries(blob_sea_src glfw assimp)

find_package(OpenGL REQUIRED)

target_link_libraries(blob_sea_src OpenGL::GL)