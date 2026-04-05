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

add_executable(MREdu
        src/main.cpp
        src/glad.c
        src/modelling/core/Entity.h
        src/modelling/core/Scene.h
        src/modelling/core/Camera.h
        src/modelling/core/components/Transform.h
        src/rendering/structs/Ray.h
        src/rendering/structs/Intersection.h
        src/modelling/core/components/Material.h
        src/modelling/core/lighting/simple/DirectionalLight.h
        src/rendering/Shader.h
        src/rendering/Shader.cpp
        src/rendering/RayTracer.cpp
        src/rendering/RayTracer.h
        src/modelling/core/primatives/Sphere.h
        src/modelling/core/primatives/Plane.h
        src/modelling/core/primatives/Cylinder.h
        src/modelling/core/lighting/simple/PointLight.h
        src/modelling/core/lighting/Light.h
        src/modelling/core/lighting/AreaLight.h
        src/rendering/structs/RenderParams.h
        src/modelling/core/lighting/area/RectangleLight.h
        src/modelling/core/lighting/LightSample.h
        src/modelling/core/BoundingBox.h
        src/modelling/core/BVH.h
        src/modelling/core/primatives/Cuboid.h
        src/modelling/core/primatives/Cone.h
        src/modelling/core/lighting/area/CuboidLight.h
        src/modelling/core/lighting/area/SphereLight.h
        src/modelling/core/lighting/area/CylinderLight.h
        src/tiny_obj_loader.h
        src/modelling/core/primatives/Triangle.h
        src/modelling/core/complex/Mesh.h
        src/modelling/core/complex/MeshLoader.h
        src/modelling/core/complex/MeshLoader.cpp
        src/modelling/core/components/Texture.h
        src/modelling/core/components/stb_image.h
        src/modelling/core/components/Texture.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui.draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(MREdu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends

)

target_link_libraries(MREdu PRIVATE
        OpenGL::GL
        glfw
        glm
)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)