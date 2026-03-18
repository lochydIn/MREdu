#include <chrono>
#include <iostream>
#include <random>
#include <vector>
#include <glad/glad.h>
#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/core/Scene.h"
#include "modelling/core/Camera.h"
#include "modelling/core/complex/Mesh.h"
#include "modelling/core/complex/MeshLoader.h"
#include "modelling/core/primatives/Sphere.h"
#include "modelling/core/primatives/Plane.h"
#include "modelling/core/lighting/simple/DirectionalLight.h"
#include "modelling/core/lighting/simple/PointLight.h"
#include "modelling/core/components/Material.h"
#include "modelling/core/lighting/area/CuboidLight.h"
#include "modelling/core/lighting/area/CylinderLight.h"
#include "modelling/core/lighting/area/RectangleLight.h"
#include "modelling/core/lighting/area/SphereLight.h"
#include "modelling/core/primatives/Cone.h"
#include "modelling/core/primatives/Cuboid.h"
#include "modelling/core/primatives/Cylinder.h"
#include "modelling/core/primatives/Sphere.h"
#include "rendering/structs/RenderParams.h"
#include "rendering/RayTracer.h"


void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

// Simple vertex shader - just draws a fullscreen rectangle
const char* vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec2 aPos;  // Input: vertex position
out vec2 TexCoord;                   // Output: texture coordinate
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);  // Position as is
    TexCoord = aPos * 0.5 + 0.5;          // Convert -1 to 1 range to 0 to 1
}
)";

// Simple fragment shader - just shows the texture
const char* fragmentShaderSource = R"(
#version 460 core
in vec2 TexCoord;                      // Input: texture coord from vertex shader
out vec4 FragColor;                     // Output: final color
uniform sampler2D screenTexture;        // The texture containing our ray traced image
void main() {
    FragColor = texture(screenTexture, vec2(TexCoord.x, 1.0 - TexCoord.y));  // Sample texture at this coordinate
}
)";

// Helper function to check shader compilation
bool checkShaderCompile(const GLuint shader, const char* name) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation failed (" << name << "): " << infoLog << std::endl;
        return false;
    }
    return true;
}


int main(int argc, char* argv[]) {
    // Setting up a simple viewport to test primitives and components.
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(960, 720, "Viewport", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 960, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Scene Test Setup
    Scene scene;

    auto brickTexture = std::make_shared<Texture>("C:/Users/Lochlan Harvey/Desktop/MREdu/src/assets/textures/rock_01_diff_4k.jpg");

    auto mat = std::make_shared<Material>(glm::vec3(0.8f,0.8f,0.8f),0.3f,0.0f,
        0.0f,0.0f,0.0f,glm::vec3(0.0f));
    mat->albedoMap = brickTexture;


    auto light = new DirectionalLight(glm::vec3(0,-1,-1),
        glm::vec3(1.0f,1.0f,1.0f),3.0f,0.05f);
    scene.addLight(light);

    auto f16 = loadObjectMesh("C:/Users/Lochlan Harvey/Desktop/MREdu/src/assets/objects/f16.obj",
        glm::vec3(0.0f,1.5f,0.0f),2.0f,mat);


    scene.addEntity(f16);

    Camera camera(
        glm::vec3(0.0f, 3.5f, 6.0f),
        glm::vec3(0.0f, 2.5f, 0.0f)
    );

    // Render Setup
    RenderParams renderParams;

    // Quality Settings. (Anti-Aliasing)
    renderParams.primarySamples = 1;
    renderParams.reflectionSamples = 1;
    renderParams.shadowSamples = 4;
    renderParams.maxDepth = 1;

    // Shadow Settings
    renderParams.softShadows = true;
    renderParams.shadowBias = 0.0001f;
    renderParams.shadowHaltonBases = {5,7};

    // Russian Roulette Settings
    renderParams.russianRoulette = true;
    renderParams.russianRoulFactor = 0.5f;
    renderParams.russianRouletteStartDepth = 3;

    // Performance / Quality
    renderParams.reduceSamplesWithDepth = true;
    renderParams.jitter = 1.0f;

    //Technical Settings
    renderParams.backgroundColor = glm::vec3(0.3f, 0.3f, 0.6f);
    renderParams.haltonBases = {2,3};

    scene.buildBVH();

    //OpenGL Texture Setup
    GLuint texture;
    // Container to store image in GPU.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Texture Params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 960, 720, 0, GL_RGB, GL_FLOAT, nullptr);

    // Fullscreen Quad
    float vertices[] = {
        // positions (x, y)
        -1.0f, -1.0f,  // bottom left
         1.0f, -1.0f,  // bottom right
        -1.0f,  1.0f,  // top left
         1.0f,  1.0f   // top right
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle (bottom left, bottom right, top left)
        1, 3, 2   // second triangle (bottom right, top right, top left)
    };

    // VAO and buffers
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Vertex attribute (just position)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);


    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    if (!checkShaderCompile(vertexShader, "vertex shader")) return -1;

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    if (!checkShaderCompile(fragmentShader, "fragment shader")) return -1;

    // Link shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader linking failed: " << infoLog << std::endl;
        return -1;
    }

    // Clean up shaders.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Prep Pixel Buffer.
    std::vector<glm::vec3> pixels(960 * 720);

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < 720; y++) {
        for (int x = 0; x < 960; x++) {
            glm::vec3 colour = RayTracer::tracePixelHalton(scene, camera,
                static_cast<float>(x), static_cast<float>(y), renderParams);
            pixels[y * 960 + x] = colour;
        }
        if (y % 72 == 0)
        {
            std::cout << "Progress: " << (y * 100 / 720) << "%" << std::endl;
        }
    }


    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration_sec = std::chrono::duration<double>(endTime - startTime);
    std::cout << "Rendering complete! Time: " << duration_sec.count() << " seconds" << std::endl;

    // Upload the final image to GPU texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 960, 720,
                    GL_RGB, GL_FLOAT, pixels.data());

    // Render Loop.
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the quad with our pre-rendered texture
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events (this will be fast now!)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
