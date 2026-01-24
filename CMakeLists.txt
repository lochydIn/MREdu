cmake_minimum_required(VERSION 4.1)
project(MREdu)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

include_directories(libs/glm-master)
add_subdirectory(libs/imgui-master)
add_subdirectory(libs/glfw-3.4)

add_executable(MREdu main.cpp)

target_link_libraries(MREdu OpenGL::GL)
