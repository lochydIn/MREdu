#include <iostream>

#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/components/Material.h"
#include "modelling/components/Sphere.h"
#include "rendering/Camera.h"
#include "rendering/DirLight.h"
#include "rendering/Shader.h"


int main(int argc, char* argv[])
{
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(480, 360, "Viewport", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    Sphere sphere(2.0f, glm::vec3(0.0f, 0.0f, -5.0f));
    Sphere sphere2(1.0f, glm::vec3(1.0f, 0.0f, -4.0f));

    Material redPlastic(glm::vec3(1,0,0),0.5f,0.0f);
    Material greenPlastic(glm::vec3(0,1,0),0.5f,0.0f);

    DirLight light(glm::vec3(0.3,0,-1),glm::vec3(1,1,1),1.0f);
    Camera camera (glm::vec3(0,0,0),glm::vec3(0,0,-5));

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-1,1,-1,1,-1,1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        glBegin(GL_POINTS);

        for (int y = 0; y < 360; y++) {
            for (int x = 0; x < 480; x++) {

                // Convert to normalized coordinates
                float nx = (2.0f * x / 480.0f) - 1.0f;
                float ny = (2.0f * y / 360.0f) - 1.0f;

                Intersection hit1, hit2;
                Ray ray = camera.getRay(x, y);

                bool hitSphere1 = sphere.intersect(ray, hit1);
                bool hitSphere2 = sphere2.intersect(ray, hit2);

                if (hitSphere2) {
                    glm::vec3 colour = Shader::shade(greenPlastic,hit2,light,ray);
                    glColor3f(colour.r,colour.g,colour.b);
                } else if (hitSphere1) {
                    glm::vec3 shadowDir = light.direction;  // Toward light (angled)
                    Ray shadowRay(hit1.point + hit1.normal*0.001f, shadowDir);

                    Intersection shadowHit;
                    bool inShadow = sphere2.intersect(shadowRay, shadowHit);

                    // Visualize shadow
                    if (inShadow && shadowHit.distance > 0.0f) {
                        float shadowFactor = (inShadow && shadowHit.distance > 0.0f) ? 0.5f : 1.0f;
                        glm::vec3 colour = Shader::shade(redPlastic, hit1, light, ray) * shadowFactor;
                        glColor3f(colour.r, colour.g, colour.b);
                    } else {
                        glm::vec3 colour = Shader::shade(redPlastic, hit1, light, ray);
                        glColor3f(colour.r, colour.g, colour.b);
                    }
                } else {
                    glColor3f(0,0,0);
                }
                glVertex2f(nx, ny);
            }
        }
        glVertex2f(0.0f, 0.0f);
        glEnd();

        glfwSwapBuffers(window);
    }

    return 0;
}
