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

add_executable(MREdu src/main.cpp
        src/modelling/core/ModelObject.cpp
        src/modelling/core/ModelObject.h
        src/modelling/core/Scene.cpp
        src/modelling/core/Scene.h
        src/modelling/components/Component.h
        src/modelling/core/SceneManager.cpp
        src/modelling/core/SceneManager.h
        src/rendering/Camera.h
        src/modelling/components/Transform.h
        src/rendering/Ray.h)

set(IMGUI_SOURCES
        $(imgui_SOURCE_DIR)/imgui.cpp
        $(imgui_SOURCE_DIR)/imgui.draw.cpp
        $(imgui_SOURCE_DIR)/imgui_tables.cpp
        $(imgui_SOURCE_DIR)/imgui_widgets.cpp
        $(imgui_SOURCE_DIR)/backends/imgui_impl_glfw.cpp
        $(imgui_SOURCE_DIR)/backends/imgui_impl_opengl3.cpp
)

target_include_directories(MREdu PRIVATE
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(MREdu PRIVATE
        OpenGL::GL
        glfw
        glm
)