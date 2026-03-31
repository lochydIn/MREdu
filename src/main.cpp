#include <chrono>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/core/Scene.h"
#include "modelling/core/Camera.h"
#include "modelling/core/complex/Mesh.h"
#include "modelling/core/complex/MeshLoader.h"
#include "modelling/core/primatives/Sphere.h"
#include "modelling/core/lighting/simple/DirectionalLight.h"
#include "modelling/core/lighting/simple/PointLight.h"
#include "modelling/core/components/Material.h"
#include "modelling/core/primatives/Cone.h"
#include "rendering/structs/RenderParams.h"
#include "rendering/RayTracer.h"


Camera* g_camera = nullptr;
constexpr int FULL_WIDTH = 960;
constexpr int FULL_HEIGHT = 720;
constexpr int PREVIEW_WIDTH = 480;
constexpr int PREVIEW_HEIGHT = 360;

struct QualityPreset {
    int pSamples;
    int sSamples;
    int rSamples;
    int maxDepth;
};

constexpr QualityPreset Interactive = {1,4,1,3};
constexpr QualityPreset Preview = {4,16,4,6};
constexpr QualityPreset Production = {16,64,8,12};

int currentWidth = PREVIEW_WIDTH;
int currentHeight = PREVIEW_HEIGHT;
QualityPreset currentQuality = Interactive;
bool render = true;

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
    render = true;
}

void key_callback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_I) {
            currentWidth = PREVIEW_WIDTH;
            currentHeight = PREVIEW_HEIGHT;
            currentQuality = Interactive;
            if (g_camera) {
                g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                    currentWidth,currentHeight);
            }
            render = true;

        } else if (key == GLFW_KEY_P) {
            currentWidth = PREVIEW_WIDTH;
            currentHeight = PREVIEW_HEIGHT;
            currentQuality = Preview;
            if (g_camera) {
                g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                    currentWidth,currentHeight);
                render = true;
            }

        } else if (key == GLFW_KEY_F) {
            currentWidth = FULL_WIDTH;
            currentHeight = FULL_HEIGHT;
            currentQuality = Production;
            if (g_camera) {
                g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                    currentWidth,currentHeight);
                render = true;
            }
        }
    }
}

// Simple fragment shader - just shows the texture
auto fragmentShaderSource = R"(
#version 460 core
in vec2 TexCoord;                      // Input: texture coord from vertex shader
out vec4 FragColor;                     // Output: final color
uniform sampler2D screenTexture;        // The texture containing our ray traced image
void main() {
    FragColor = texture(screenTexture, vec2(TexCoord.x, 1.0 - TexCoord.y));  // Sample texture at this coordinate
}
)";

// Simple vertex shader - just draws a fullscreen rectangle
auto vertexShaderSource = R"(
#version 460 core
layout(location = 0) in vec2 aPos;  // Input: vertex position
out vec2 TexCoord;                   // Output: texture coordinate
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);  // Position as is
    TexCoord = aPos * 0.5 + 0.5;          // Convert -1 to 1 range to 0 to 1
}
)";

// Helper function to check shader compilation
bool checkShaderCompile(const GLuint shader, const char* name) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Shader compilation failed (" << name << "): " << infoLog << std::endl;
        return false;
    }
    return true;
}



void renderScene(const Scene& scene, const Camera& camera, const RenderParams& renderParams,
    std::vector<glm::vec3>& pixels, const int width, const int height) {

    const auto start = std::chrono::high_resolution_clock::now();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const glm::vec3 colour = RayTracer::tracePixelHalton(scene,camera,static_cast<float>(x),
                static_cast<float>(y),renderParams);
            pixels[y * width + x] = colour;
        }
        if (y % (height / 10) == 0 && height > 0) {
            std::cout << (y * 100 / height) << "%" << std::endl;
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Rendered in: " << duration_sec.count() << " seconds" << std::endl;
}


int main(int argc, char* argv[]) {
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(FULL_WIDTH, FULL_HEIGHT,
        "Viewport", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, FULL_WIDTH, FULL_HEIGHT);

    // Render Setup
    RenderParams renderParams;

    // Quality Settings. (Anti-Aliasing)
    renderParams.primarySamples = currentQuality.pSamples;
    renderParams.reflectionSamples = currentQuality.rSamples;
    renderParams.shadowSamples = currentQuality.sSamples;
    renderParams.maxDepth = currentQuality.maxDepth;

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

    // Scene Setup
    Scene scene;
    auto brickAlbedo = std::make_shared<Texture>("src/assets/textures/Bricks101_1K-JPG_Color.jpg");
    auto brickRoughness = std::make_shared<Texture>("src/assets/textures/Bricks101_1K-JPG_Roughness.jpg");
    auto brickNormal = std::make_shared<Texture>("src/assets/textures/Bricks101_1K-JPG_NormalGL.jpg");
    auto brickAO = std::make_shared<Texture>("src/assets/textures/Bricks101_1K-JPG_AmbientOcclusion.jpg");

    auto testMat = std::make_shared<Material>(glm::vec3(0.8f,0.8f,0.8f),0.7f,0.0f,
        0.0f,0.0f,0.0f,glm::vec3(0.0f));

    testMat->albedoMap = brickAlbedo;
    testMat->roughnessMap = brickRoughness;
    testMat->normalMap = brickNormal;
    testMat->aoMap = brickAO;

    auto testCarPaint = std::make_shared<Material>(glm::vec3(1.0f,0.2f,0.2f),
        0.3f,1.0f,0.0,0,0.0f,glm::vec3(0.0f));
    testCarPaint->clearcoat = 1.0f;
    testCarPaint->clearcoatRoughness = 0.5f;

    auto velvet = std::make_shared<Material>(glm::vec3(0.7f,0.2f,0.3f),
        0.8f,0.0f,0.0f,0.0f,0.0f,glm::vec3(0.0f));
    velvet->sheen = 1.0f;
    velvet->sheenColour = glm::vec3(0.9f,0.3f,0.4f);

    auto brushedMetal = std::make_shared<Material>();
    brushedMetal->colour = glm::vec3(0.9f,0.8f,0.6f);
    brushedMetal->roughness = 0.2f;
    brushedMetal->metallic = 1.0f;
    brushedMetal->anisotropy = 0.9f;
    brushedMetal->anisotropyRotation = 90.0f;


    auto light = new DirectionalLight(glm::vec3(0,-1.0,-1.0f),
        glm::vec3(1.0f,1.0f,1.0f),3.0f,0.01f);
    scene.addLight(light);

    Mesh* bunny = loadObjectMesh("src/assets/objects/buddha.obj",
        glm::vec3(0.0f,0.0f,0.0f),20.0f, velvet);

    scene.addEntity(bunny);
    Camera camera(
        glm::vec3(0.0f, 2.5f, 6.0f),
        glm::vec3(0.0f, 2.5f, 0.0f),60,currentWidth,currentHeight
    );
    g_camera = &camera;

    scene.buildBVH();


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
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    if (!checkShaderCompile(vertexShader, "vertex shader")) return -1;

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
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
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shader linking failed: " << infoLog << std::endl;
        return -1;
    }

    // Clean up shaders.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
        currentWidth, currentHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    std::vector<glm::vec3> pixels(currentWidth * currentHeight);



    renderScene(scene,camera,renderParams,pixels,currentWidth,currentHeight);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, currentWidth, currentHeight, GL_RGB, GL_FLOAT, pixels.data());
    render = false;
    while (!glfwWindowShouldClose(window)) {
        if (render) {
            renderParams.primarySamples = currentQuality.pSamples;
            renderParams.reflectionSamples = currentQuality.rSamples;
            renderParams.shadowSamples = currentQuality.sSamples;
            renderParams.maxDepth = currentQuality.maxDepth;

            pixels.resize(currentWidth * currentHeight);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
                currentWidth, currentHeight, 0 , GL_RGB, GL_FLOAT, nullptr);

            renderScene(scene,camera,renderParams,pixels,currentWidth,currentHeight);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                currentWidth, currentHeight, GL_RGB, GL_FLOAT,pixels.data());

            render = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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
