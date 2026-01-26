#include <iostream>

#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"


int main(int
         argc, char* argv[])
{
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(480, 360, "Viewport", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}