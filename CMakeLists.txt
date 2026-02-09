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
        src/modelling/core/Entity.h
        src/modelling/components/Component.h
        src/modelling/core/Scene.h
        src/modelling/core/Camera.h
        src/modelling/components/Transform.h
        src/rendering/Ray.h
        src/rendering/Intersection.h
        src/modelling/components/Material.h
        src/modelling/core/lighting/DirectionalLight.h
        src/rendering/Shader.h
        src/rendering/Shader.cpp
        src/rendering/RayTracer.cpp
        src/rendering/RayTracer.h
        src/modelling/core/primatives/Sphere.h
        src/modelling/core/primatives/plane.h
        src/modelling/core/primatives/cone.h
        src/modelling/core/primatives/cylinder.h
        src/modelling/core/lighting/pointLight.h
        src/modelling/core/lighting/Light.h)

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
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)