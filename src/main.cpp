#include <iostream>

#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/components/Material.h"
#include "modelling/core/primatives/Sphere.h"
#include "modelling/core/Camera.h"
#include "modelling/core/lighting/DirectionalLight.h"
#include "modelling/core/Scene.h"
#include "modelling/core/lighting/PointLight.h"
#include "rendering/RayTracer.h"
#include "rendering/Shader.h"
#include "modelling/core/primatives/Plane.h"


int main(int argc, char* argv[])
{
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(960, 720, "Viewport", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    Scene scene;

    auto redMaterial = std::make_shared<Material>(glm::vec3(1,0,0),0.0f,0.0f,0.3f);
    auto greenMaterial = std::make_shared<Material>(glm::vec3(0,1,0),0.0f,0.0f,0.3f);
    auto blueMaterial = std::make_shared<Material>(glm::vec3(0,0,1),0.0f,0.0f,0.3f);
    auto mirrorMaterial = std::make_shared<Material>(glm::vec3(1,1,1),0.0f,1.0f,0.8f);
    auto floorMat = std::make_shared<Material>(glm::vec3(0.8),0.5f,0.0f,0.1f);

    auto* redSphere = new Sphere(1.0f, glm::vec3 (-2.5, 0.0f, -5.0f),redMaterial);
    auto* greenSphere = new Sphere(1.0f, glm::vec3 (0.0f, 0.0f, -6.0f),greenMaterial);
    auto* blueSphere = new Sphere(1.0f, glm::vec3 (2.5f, 0.0f, -7.0f),blueMaterial);

    auto* mirrorSphere = new Sphere(2, glm::vec3(0.0f,2.0f,-9.0f),mirrorMaterial);

    auto* floor = new Plane(glm::vec3(0,-2,0),glm::vec3(0,1,0), floorMat);


    scene.addEntity(redSphere);
    scene.addEntity(greenSphere);
    scene.addEntity(blueSphere);
    scene.addEntity(mirrorSphere);
    scene.addEntity(floor);

    scene.addLight(new DirectionalLight(
        glm::vec3(0,-1,0),
        glm::vec3(1,1,1),
        1.0f));


    Camera camera (glm::vec3(0,1,0),glm::vec3(0,0,-5));

    auto tracer = RayTracer();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1,1,-1,1,-1,1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_POINTS);

        for (int y = 0; y < 720; y++) {
            for (int x = 0; x < 960; x++) {

                // Convert to normalized coordinates
                float nx = (2.0f * x / 960.0f) - 1.0f;
                float ny = 1.0f - (2.0f * y / 720.0f);

                Ray ray = camera.getRay(x, y);
                glm::vec3 colour;
                colour = tracer.trace(scene,ray,0);
                glColor3f (colour.r, colour.g, colour.b);

                glVertex2f(nx, ny);
            }
        }
        glEnd();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
