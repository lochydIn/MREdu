cmake_minimum_required(VERSION 4.1)
project(MREdu)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
)

FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.5
)

FetchContent_MakeAvailable(glm glfw imgui)

add_executable(MREdu main.cpp imgui_impl_glfw.cpp imgui_impl_opengl3.cpp)

link_libraries(MREdu
        OpenGL::GL
        glm
        glfw
        imgui
)