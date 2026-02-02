#include <iostream>

#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/components/Sphere.h"
#include "rendering/Camera.h"


int main(int
         argc, char* argv[])
{
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(480, 360, "Viewport", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    Sphere sphere("sphere", 2.0f, glm::vec3(0.0f, 0.0f, -5.0f));

    Camera camera (glm::vec3(0,0,0),
        glm::vec3(0,0,-5));

    while (!glfwWindowShouldClose(window)) {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,480,0,360,-1,1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_POINTS);


        for (int y = 0; y < 360 ; y++) {
            for (int x = 0; x < 480 ; x++) {

                Intersection intersection;
                if (Ray ray = camera.getRay(x,y); sphere.intersect(ray,intersection)) {
                    glColor3f(1.0, 0.0, 0.0);
                } else {
                    glColor3f(0.0, 0.0, 1.0);
                    }
                glVertex2d(x,y);
            }
        }
        glEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
