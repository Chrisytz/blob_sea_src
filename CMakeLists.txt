cmake_minimum_required(VERSION 3.17)
project(blob_sea_src)

set(CMAKE_CXX_STANDARD 14)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

include_directories(${PARENT_DIR}/Blob_Lib/Include
                    Inc
                    Resources
                    Src)

add_subdirectory(${PARENT_DIR}/Blob_Lib/glfw-3.3.7/glfw glfw)
add_subdirectory(${PARENT_DIR}/Blob_Lib/assimp-5.2.3/assimp assimp)

link_directories(${PARENT_DIR}/Blob_Lib/Libs)

add_executable(blob_sea_src glad.c main.cpp Inc/shader.h Src/shader.cpp Inc/stb_image.h Src/stb_image.cpp Inc/camera.h Src/camera.cpp Inc/mesh.h Src/mesh.cpp Inc/model.h Src/model.cpp)

# Linking GLFW
target_link_libraries(blob_sea_src glfw assimp)

find_package(OpenGL REQUIRED)

target_link_libraries(blob_sea_src OpenGL::GL)