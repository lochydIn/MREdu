#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>

#include "imgui_internal.h"
#include "../cmake-build-debug/_deps/glfw-src/include/GLFW/glfw3.h"
#include "modelling/core/Scene.h"
#include "modelling/core/Camera.h"
#include "modelling/core/meshes/Mesh.h"
#include "modelling/core/meshes/MeshLoader.h"
#include "modelling/core/primatives/Sphere.h"
#include "modelling/core/lighting/simple/DirectionalLight.h"
#include "modelling/core/lighting/simple/PointLight.h"
#include "modelling/core/components/Material.h"
#include "modelling/core/lighting/area/CuboidLight.h"
#include "modelling/core/lighting/area/CylinderLight.h"
#include "modelling/core/lighting/area/SphereLight.h"
#include "modelling/core/primatives/Cone.h"
#include "modelling/core/primatives/Cuboid.h"
#include "modelling/core/primatives/Cylinder.h"
#include "rendering/structs/RenderParams.h"
#include "rendering/RayTracer.h"


Camera* g_camera = nullptr;
constexpr int PRODUCTION_WIDTH = 1920;
constexpr int PRODUCTION_HEIGHT = 1080;
constexpr int PREVIEW_WIDTH = 1280;
constexpr int PREVIEW_HEIGHT = 720;
constexpr int INTERACTIVE_WIDTH = 640;
constexpr int INTERACTIVE_HEIGHT = 360;

struct QualityPreset
{
    int pSamples;
    int sSamples;
    int rSamples;
    int maxDepth;
};

constexpr QualityPreset INTERACTIVE = {1, 1, 1, 3};
constexpr QualityPreset PREVIEW = {4, 8, 4, 4};
constexpr QualityPreset PRODUCTION = {8, 64, 12, 8};

int currentWidth = INTERACTIVE_WIDTH;
int currentHeight = INTERACTIVE_HEIGHT;
QualityPreset currentQuality = INTERACTIVE;
bool render = true;
bool cancelRender = false;
float cameraSpeed = 0.5f;
bool cameraMode = false;

enum ApplicationState { MENU, RUNNING };
ApplicationState state = MENU;


void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
    render = true;
}

void key_callback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_C)
        {
            cameraMode = !cameraMode;
            glfwSetInputMode(window,GLFW_CURSOR, cameraMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            if (cameraMode)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::cout << "Camera mode ON" << std::endl;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "Camera mode OFF" << std::endl;
            }
        }
        else if (key == GLFW_KEY_S)
        {
            cancelRender = true;
        }
    }
}

void mouse_callback(GLFWwindow* window, const double xPos, const double yPos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
    if (!cameraMode || state != RUNNING) return;

    static int callCount = 0;
    if (callCount++ < 10)
    {
        std::cout << "Mouse callback called: " << xPos << ", " << yPos << std::endl;
    }


    static double lastX = PRODUCTION_WIDTH / 2.0;
    static double lastY = PRODUCTION_WIDTH / 2.0;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    const float xOffset = (lastX - xPos) * 0.0005;
    const float yOffset = (lastY - yPos) * 0.0005;

    lastX = xPos;
    lastY = yPos;

    g_camera->look(xOffset, yOffset);
    render = true;
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
bool checkShaderCompile(const GLuint shader, const char* name)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Shader compilation failed (" << name << "): " << infoLog << std::endl;
        return false;
    }
    return true;
}

void renderScene(const Scene& scene, const Camera& camera, const RenderParams& renderParams,
                 std::vector<glm::vec3>& pixels, const int width, const int height)
{
    const auto start = std::chrono::high_resolution_clock::now();
    cancelRender = false;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            glfwPollEvents();
            if (cancelRender)
            {
                std::cout << "Render cancelled." << std::endl;
                return;
            }
            const glm::vec3 colour = RayTracer::tracePixelHalton(scene, camera, static_cast<float>(x),
                                                                 static_cast<float>(y), renderParams);
            pixels[y * width + x] = colour;
        }
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Rendered in: " << duration_sec.count() << " seconds" << std::endl;
}

int main(int argc, char* argv[])
{
    // Setting up a simple viewport to test primitives and components.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(PRODUCTION_WIDTH, PRODUCTION_HEIGHT,
                                          "Viewport", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, PRODUCTION_WIDTH, PRODUCTION_HEIGHT);

    // Render Setup
    RenderParams renderParams;
    // Quality Settings. (Anti-Aliasing)
    renderParams.primarySamples = currentQuality.pSamples;
    renderParams.reflectionSamples = currentQuality.rSamples;
    renderParams.shadowSamples = currentQuality.sSamples;
    renderParams.maxDepth = currentQuality.maxDepth;
    // Shadow Settings
    renderParams.softShadows = true;
    renderParams.shadowBias = 0.001f;
    renderParams.shadowHaltonBases = {5, 7};
    // Russian Roulette Settings
    renderParams.russianRoulette = true;
    renderParams.russianRoulFactor = 0.5f;
    renderParams.russianRouletteStartDepth = 3;
    // Performance / Quality
    renderParams.reduceSamplesWithDepth = true;
    renderParams.jitter = 1.0f;
    //Technical Settings
    renderParams.backgroundColor = glm::vec3(0.9f, 0.9f, 0.9f);
    renderParams.haltonBases = {2, 3};
    // Scene Setup
    Scene scene;
    Camera camera(
        glm::vec3(0.0f, 0.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), 60, currentWidth, currentHeight
    );
    g_camera = &camera;
    scene.buildBVH();

    // Fullscreen Quad
    float vertices[] = {
        // positions (x, y)
        -1.0f, -1.0f, // bottom left
        1.0f, -1.0f, // bottom right
        -1.0f, 1.0f, // top left
        1.0f, 1.0f // top right
    };
    unsigned int indices[] = {
        0, 1, 2, // first triangle (bottom left, bottom right, top left)
        1, 3, 2 // second triangle (bottom right, top right, top left)
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
    if (!success)
    {
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

    // Some initial variables.
    std::vector<glm::vec3> pixels(currentWidth * currentHeight);
    render = false;
    bool showCustomRenderSettings = false;
    Entity* selectedEntity = nullptr;
    Light* selectedLight = nullptr;

    // Texture Presets
    std::vector<TexturePreset> texturePresets = {
        {
            "GreyBricks",
            "src/assets/textures/Bricks066-GreyBricksLrg/Bricks066_1K-JPG_Color.jpg",
            "src/assets/textures/Bricks066-GreyBricksLrg/Bricks066_1K-JPG_Roughness.jpg",
            "src/assets/textures/Bricks066-GreyBricksLrg/Bricks066_1K-JPG_NormalGL.jpg",
            "",
            "",
            "src/assets/textures/Bricks066-GreyBricksLrg/Bricks066_1K-JPG_AmbientOcclusion.jpg"
        },
        {"RedBricks",
            "src/assets/textures/Bricks082A-RedBricks/Bricks082A_1K-JPG_Color.jpg",
            "src/assets/textures/Bricks082A-RedBricks/Bricks082A_1K-JPG_Roughness.jpg",
            "src/assets/textures/Bricks082A-RedBricks/Bricks082A_1K-JPG_NormalGL.jpg",
            "",
            "",
            "src/assets/textures/Bricks082A-RedBricks/Bricks082A_1K-JPG_AmbientOcclusion.jpg"
        },
        {"SandstoneBricks",
            "src/assets/textures/Bricks084-Sandstone/Bricks084_1K-JPG_Color.jpg",
            "src/assets/textures/Bricks084-Sandstone/Bricks084_1K-JPG_Roughness.jpg",
            "src/assets/textures/Bricks084-Sandstone/Bricks084_1K-JPG_NormalGL.jpg",
            "",
            "",
            "src/assets/textures/Bricks084-Sandstone/Bricks084_1K-JPG_AmbientOcclusion.jpg"
        },
        {"BlueCheckCarpet",
            "src/assets/textures/Carpet006-BlueCheckCarpet/Carpet006_1K-JPG_Color.jpg",
            "src/assets/textures/Carpet006-BlueCheckCarpet/Carpet006_1K-JPG_Roughness.jpg",
            "src/assets/textures/Carpet006-BlueCheckCarpet/Carpet006_1K-JPG_NormalGL.jpg",
            "",
            "",
            "src/assets/textures/Carpet006-BlueCheckCarpet/Carpet006_1K-JPG_AmbientOcclusion.jpg"
        },
        {"BeigeCarpet",
            "src/assets/textures/Carpet016-BeigeCarpet/Carpet016_1K-JPG_Color.jpg",
            "src/assets/textures/Carpet016-BeigeCarpet/Carpet016_1K-JPG_Roughness.jpg",
            "src/assets/textures/Carpet016-BeigeCarpet/Carpet016_1K-JPG_NormalGL.jpg",
            "",
            "",
            "src/assets/textures/Carpet016-BeigeCarpet/Carpet016_1K-JPG_AmbientOcclusion.jpg"
        },
        {"DiamondFabric",
        "src/assets/textures/Fabric008-DiamondFabric/Fabric008_1K-JPG_Color.jpg",
        "src/assets/textures/Fabric008-DiamondFabric/Fabric008_1K-JPG_Roughness.jpg",
        "src/assets/textures/Fabric008-DiamondFabric/Fabric008_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/Fabric008-DiamondFabric/Fabric008_1K-JPG_AmbientOcclusion.jpg"},
        {"CheckCottonFabric",
        "src/assets/textures/Fabric080-CheckCotton/Fabric080_1K-JPG_Color.jpg",
        "src/assets/textures/Fabric080-CheckCotton/Fabric080_1K-JPG_Roughness.jpg",
        "src/assets/textures/Fabric080-CheckCotton/Fabric080_1K-JPG_NormalGL.jpg",
        "",
        "",
            ""},
        {"Granite",
        "src/assets/textures/Granite002A-Granite/Granite002A_1K-JPG_Color.jpg",
        "src/assets/textures/Granite002A-Granite/Granite002A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Granite002A-Granite/Granite002A_1K-JPG_NormalGL.jpg",
        "",
        "",
        ""},
        {"Mud",
        "src/assets/textures/Ground071-Mud/Ground071_1K-JPG_Color.jpg",
        "src/assets/textures/Ground071-Mud/Ground071_1K-JPG_Roughness.jpg",
        "src/assets/textures/Ground071-Mud/Ground071_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/Ground071-Mud/Ground071_1K-JPG_AmbientOcclusion.jpg"},
        {"Sand",
        "src/assets/textures/Ground089-Sand/Ground089_1K-JPG_Color.jpg",
        "src/assets/textures/Ground089-Sand/Ground089_1K-JPG_Roughness.jpg",
        "src/assets/textures/Ground089-Sand/Ground089_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/Ground089-Sand/Ground089_1K-JPG_AmbientOcclusion.jpg"},
        {"ScratchedGold",
        "src/assets/textures/Metal007-ScratchedGold/Metal007_1K-JPG_Color.jpg",
        "src/assets/textures/Metal007-ScratchedGold/Metal007_1K-JPG_Roughness.jpg",
        "src/assets/textures/Metal007-ScratchedGold/Metal007_1K-JPG_NormalGL.jpg",
        "",
            "src/assets/textures/Metal007-ScratchedGold/Metal007_1K-JPG_Metalness.jpg",
            ""},
        {"RustedSteel",
        "src/assets/textures/Metal024-RustedSteel/Metal024_1K-JPG_Color.jpg",
        "src/assets/textures/Metal024-RustedSteel/Metal024_1K-JPG_Roughness.jpg",
        "src/assets/textures/Metal024-RustedSteel/Metal024_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Metal024-RustedSteel/Metal024_1K-JPG_Metalness.jpg",
            ""},
        {"Copper",
        "src/assets/textures/Metal035-Copper/Metal035_1K-JPG_Color.jpg",
        "src/assets/textures/Metal035-Copper/Metal035_1K-JPG_Roughness.jpg",
        "src/assets/textures/Metal035-Copper/Metal035_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Metal035-Copper/Metal035_1K-JPG_Metalness.jpg",
        ""},
        {"PolishedGold",
        "src/assets/textures/Metal048-PolishedGold/Metal048A_1K-JPG_Color.jpg",
        "src/assets/textures/Metal048-PolishedGold/Metal048A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Metal048-PolishedGold/Metal048A_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Metal048-PolishedGold/Metal048A_1K-JPG_Metalness.jpg",
            ""},
        {"ShinyCopper",
        "src/assets/textures/Metal057-ShinyCopper/Metal057A_1K-JPG_Color.jpg",
        "src/assets/textures/Metal057-ShinyCopper/Metal057A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Metal057-ShinyCopper/Metal057A_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Metal057-ShinyCopper/Metal057A_1K-JPG_Metalness.jpg",
        ""},
        {"TriangleMetalPlating",
        "src/assets/textures/MetalPlates006-TriMetalPlates/MetalPlates006_1K-JPG_Color.jpg",
        "src/assets/textures/MetalPlates006-TriMetalPlates/MetalPlates006_1K-JPG_Roughness.jpg",
        "src/assets/textures/MetalPlates006-TriMetalPlates/MetalPlates006_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/MetalPlates006-TriMetalPlates/MetalPlates006_1K-JPG_Metalness.jpg",
        ""},
        {"SquareMetalPlating",
        "src/assets/textures/MetalPlates007-SqrMetalPlates/MetalPlates007_1K-JPG_Color.jpg",
        "src/assets/textures/MetalPlates007-SqrMetalPlates/MetalPlates007_1K-JPG_Roughness.jpg",
        "src/assets/textures/MetalPlates007-SqrMetalPlates/MetalPlates007_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/MetalPlates007-SqrMetalPlates/MetalPlates007_1K-JPG_Metalness.jpg",
        ""},
        {"OldPaintedWood",
        "src/assets/textures/PaintedWood007B-WPOldPainted/PaintedWood007B_1K-JPG_Color.jpg",
        "src/assets/textures/PaintedWood007B-WPOldPainted/PaintedWood007B_1K-JPG_Roughness.jpg",
        "src/assets/textures/PaintedWood007B-WPOldPainted/PaintedWood007B_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/PaintedWood007B-WPOldPainted/PaintedWood007B_1K-JPG_AmbientOcclusion.jpg"},
        {"PavingStone",
        "src/assets/textures/PavingStones119-CobbleStone/PavingStones119_1K-JPG_Color.jpg",
        "src/assets/textures/PavingStones119-CobbleStone/PavingStones119_1K-JPG_Roughness.jpg",
        "src/assets/textures/PavingStones119-CobbleStone/PavingStones119_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/PavingStones119-CobbleStone/PavingStones119_1K-JPG_AmbientOcclusion.jpg"},
        {"PaintedWood",
        "src/assets/textures/Planks010-WPPainted/Planks010_1K-JPG_Color.jpg",
        "src/assets/textures/Planks010-WPPainted/Planks010_1K-JPG_Roughness.jpg",
        "src/assets/textures/Planks010-WPPainted/Planks010_1K-JPG_NormalGL.jpg",
        "",
        "",
        "src/assets/textures/Planks010-WPPainted/Planks010_1K-JPG_AmbientOcclusion.jpg"},
        {"OutdoorWood",
        "src/assets/textures/Planks023B-WP-Outdour/Planks023B_1K-JPG_Color.jpg",
        "src/assets/textures/Planks023B-WP-Outdour/Planks023B_1K-JPG_Roughness.jpg",
        "src/assets/textures/Planks023B-WP-Outdour/Planks023B_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Planks023B-WP-Outdour/Planks023B_1K-JPG_Metalness.jpg",
        "src/assets/textures/Planks023B-WP-Outdour/Planks023B_1K-JPG_AmbientOcclusion.jpg"},
        {"ScrewedWood",
        "src/assets/textures/Planks037A-WPWithScrews/Planks037A_1K-JPG_Color.jpg",
        "src/assets/textures/Planks037A-WPWithScrews/Planks037A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Planks037A-WPWithScrews/Planks037A_1K-JPG_NormalGL.jpg",
        "",
        "src/assets/textures/Planks037A-WPWithScrews/Planks037A_1K-JPG_Metalness.jpg",
        "src/assets/textures/Planks037A-WPWithScrews/Planks037A_1K-JPG_AmbientOcclusion.jpg"},
        {"ScratchedPlastic",
        "src/assets/textures/Plastic005-ScratchedPlastic/Plastic005_1K-JPG_Color.jpg",
        "src/assets/textures/Plastic005-ScratchedPlastic/Plastic005_1K-JPG_Roughness.jpg",
        "src/assets/textures/Plastic005-ScratchedPlastic/Plastic005_1K-JPG_NormalGL.jpg",
        "","",""},
        {"WhitePlastic",
        "src/assets/textures/Plastic013A-WhitePlastic/Plastic013A_1K-JPG_Color.jpg",
        "src/assets/textures/Plastic013A-WhitePlastic/Plastic013A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Plastic013A-WhitePlastic/Plastic013A_1K-JPG_NormalGL.jpg",
        "","",""},
        {"GreenPlastic",
        "src/assets/textures/Plastic017A-GreenPlastic/Plastic017A_1K-JPG_Color.jpg",
        "src/assets/textures/Plastic017A-GreenPlastic/Plastic017A_1K-JPG_Roughness.jpg",
        "src/assets/textures/Plastic017A-GreenPlastic/Plastic017A_1K-JPG_NormalGL.jpg",
        "","",""},
        {"YellowSponge",
        "src/assets/textures/Sponge001-YellowSponge/Sponge001_1K-JPG_Color.jpg",
        "src/assets/textures/Sponge001-YellowSponge/Sponge001_1K-JPG_Roughness.jpg",
        "src/assets/textures/Sponge001-YellowSponge/Sponge001_1K-JPG_NormalGL.jpg",
        "","",
        "src/assets/textures/Sponge001-YellowSponge/Sponge001_1K-JPG_AmbientOcclusion.jpg"},
        {"InteriorOakWood",
        "src/assets/textures/Wood049-InteriorOak/Wood049_1K-JPG_Color.jpg",
        "src/assets/textures/Wood049-InteriorOak/Wood049_1K-JPG_Roughness.jpg",
        "src/assets/textures/Wood049-InteriorOak/Wood049_1K-JPG_NormalGL.jpg",
        "","",""},
        {"InteriorWoodFloor",
        "src/assets/textures/WoodFloor062-WPInterior/WoodFloor062_1K-JPG_Color.jpg",
        "src/assets/textures/WoodFloor062-WPInterior/WoodFloor062_1K-JPG_Roughness.jpg",
        "src/assets/textures/WoodFloor062-WPInterior/WoodFloor062_1K-JPG_NormalGL.jpg",
        "","",""},
        {"CheckerTile",
        "src/assets/textures/Tiles012-CheckerBoard/Tiles012_1K-JPG_Color.jpg",
        "src/assets/textures/Tiles012-CheckerBoard/Tiles012_1K-JPG_Roughness.jpg",
        "src/assets/textures/Tiles012-CheckerBoard/Tiles012_1K-JPG_NormalGL.jpg",
        "",
        "",
        ""}
    };


    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (state == MENU)
        {
            ImGui::SetNextWindowPos(ImVec2(PRODUCTION_WIDTH * 0.5 - 150, PREVIEW_HEIGHT * 0.5 - 100));
            ImGui::SetNextWindowSize(ImVec2(300, 200));
            ImGui::Begin("MREdu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            ImGui::SetCursorPosX(100);
            ImGui::Text("     MREdu");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SetCursorPosX(100);
            if (ImGui::Button("Start", ImVec2(100, 40)))
            {
                state = RUNNING;
                render = true;
            }
            ImGui::Spacing();
            ImGui::SetCursorPosX(100);
            if (ImGui::Button("Exit", ImVec2(100, 40)))
            {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();
        }
        else if (state == RUNNING)
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("Render Settings");
            if (ImGui::CollapsingHeader("Mode", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("Interactive"))
                {
                    showCustomRenderSettings = false;
                    currentWidth = INTERACTIVE_WIDTH;
                    currentHeight = INTERACTIVE_HEIGHT;
                    currentQuality = INTERACTIVE;
                    if (g_camera)
                    {
                        g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                                            currentWidth, currentHeight);
                    }
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Preview"))
                {
                    showCustomRenderSettings = false;
                    currentWidth = PREVIEW_WIDTH;
                    currentHeight = PREVIEW_HEIGHT;
                    currentQuality = PREVIEW;
                    if (g_camera)
                    {
                        g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                                            currentWidth, currentHeight);
                    }
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Production"))
                {
                    showCustomRenderSettings = false;
                    currentWidth = PRODUCTION_WIDTH;
                    currentHeight = PRODUCTION_HEIGHT;
                    currentQuality = PRODUCTION;
                    if (g_camera)
                    {
                        g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                                            currentWidth, currentHeight);
                        render = true;
                    }
                }
                if (ImGui::Button("Custom"))
                {
                    showCustomRenderSettings = true;
                }

                if (showCustomRenderSettings)
                {
                    ImGui::Indent();
                    ImGui::SliderInt("Primary Samples", &renderParams.primarySamples, 1, 64);
                    ImGui::SliderInt("Shadow Samples", &renderParams.shadowSamples, 1, 256);
                    ImGui::SliderInt("Reflection and Refraction Samples", &renderParams.reflectionSamples, 1, 256);
                    ImGui::SliderInt("Reflection Depth", &renderParams.maxDepth, 1, 32);
                    ImGui::Checkbox("Enable Soft Shadows", &renderParams.softShadows);
                    ImGui::Checkbox("Russian Roulette Ray Culling", &renderParams.russianRoulette);
                    ImGui::SliderInt("Roulette Start Depth", &renderParams.russianRouletteStartDepth, 0, 32);
                    ImGui::Checkbox("Reduce Samples With Depth", &renderParams.reduceSamplesWithDepth);
                    ImGui::ColorEdit3("Background Colour", &renderParams.backgroundColor.x);
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    ImGui::Text("Resolution:  %d x %d" ,currentWidth, currentHeight);
                    if (ImGui::Button("1080p"))
                    {
                        currentWidth = 1920;
                        currentHeight = 1080;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("720p"))
                    {
                        currentWidth = 1280;
                        currentHeight = 720;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("640p"))
                    {
                        currentWidth = 640;
                        currentHeight = 360;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("256p"))
                    {
                        currentWidth = 256;
                        currentHeight = 144;
                    }
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    if (ImGui::Button("Apply Settings"))
                    {
                        render = true;
                    };
                    ImGui::Unindent();
                }
            }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(10, 740));
            ImGui::Begin("Camera Tracker");
            ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)",
                camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
            ImGui::Text("Camera Target: (%.1f,%.1f,%.1f)",camera.getTarget().x,camera.getTarget().y,
                camera.getTarget().z);
            ImGui::Separator();
            ImGui::Text("Manual Override");
            glm::vec3 pos = camera.getPosition();
            glm::vec3 tgt = camera.getTarget();
            float fov = camera.getFov();

            bool changed = false;

            // Input with step buttons (users can type or use up/down arrows)
            ImGui::Text("Position");
            changed |= ImGui::InputFloat("X", &pos.x, 0.1f, 1.0f);
            changed |= ImGui::InputFloat("Y", &pos.y, 0.1f, 1.0f);
            changed |= ImGui::InputFloat("Z", &pos.z, 0.1f, 1.0f);

            ImGui::Text("Target");
            changed |= ImGui::InputFloat("TX", &tgt.x, 0.1f, 1.0f);
            changed |= ImGui::InputFloat("TY", &tgt.y, 0.1f, 1.0f);
            changed |= ImGui::InputFloat("TZ", &tgt.z, 0.1f, 1.0f);

            ImGui::Text("Field of View");
            changed |= ImGui::InputFloat("FOV",&fov,1.0f,5.0f,"%.1f");
            if (changed) {
                camera.setPosition(pos);
                camera.setTarget(tgt);
                camera.setFov(fov);
                render = true;
            }
            ImGui::End();
            ImGui::SetNextWindowPos(ImVec2(PRODUCTION_WIDTH - 320, 10));
            ImGui::Begin("Scene Explorer", nullptr, ImGuiWindowFlags_NoCollapse);

            ImGui::BeginChild("Objects (Click to add)", ImVec2(0, 300));
            if (ImGui::CollapsingHeader("Primitives", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("Sphere"))
                {
                    auto* sphere = new Sphere(std::make_shared<Material>());
                    scene.addEntity(sphere);
                    scene.buildBVH();
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cuboid"))
                {
                    auto* cuboid = new Cuboid(std::make_shared<Material>());
                    scene.addEntity(cuboid);
                    scene.buildBVH();
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Cylinder"))
                {
                    auto* cylinder = new Cylinder(0.5, 1, std::make_shared<Material>());
                    scene.addEntity(cylinder);
                    scene.buildBVH();
                    render = true;
                }
                if (ImGui::Button("Cone"))
                {
                    auto* cylinder = new Cone(1, 0.5, std::make_shared<Material>());
                    scene.addEntity(cylinder);
                    scene.buildBVH();
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Plane"))
                {
                    auto* plane = new Plane(glm::vec3(0.0f),
                                            glm::vec3(0.0f, 1.0f, 0.0f), std::make_shared<Material>());
                    scene.addEntity(plane);
                    scene.buildBVH();
                    render = true;
                }
            }
            if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Button("DirectionalLight"))
                {
                    auto* light = new DirectionalLight(glm::vec3(0.0f, -1.0f, 0.0f),
                                                       glm::vec3(1.0f), 1.0f, 0.05f);
                    scene.addLight(light);
                    scene.buildBVH();
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("PointLight"))
                {
                    auto* pointLight = new PointLight(glm::vec3(0.0f, 0.0f, 0.0f),
                                                      glm::vec3(1.0f), 1.0f, 1.0f);
                    scene.addLight(pointLight);
                    scene.buildBVH();
                    render = true;
                }
                if (ImGui::Button("CuboidLight"))
                {
                    auto* cuboidLight = new CuboidLight();
                    scene.addEntity(cuboidLight);
                    scene.addLight(cuboidLight);
                    scene.buildBVH();
                    render = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("CylinderLight"))
                {
                    auto* cylinderLight = new CylinderLight();
                    scene.addEntity(cylinderLight);
                    scene.addLight(cylinderLight);
                    scene.buildBVH();
                    render = true;
                }
                if (ImGui::Button("SphereLight"))
                {
                    auto* sphereLight = new SphereLight();
                    scene.addEntity(sphereLight);
                    scene.addLight(sphereLight);
                    scene.buildBVH();
                    render = true;
                }
            }

            if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen))
            {
                std::string assetsFolder = "src/assets/objects/";
                for (const auto& file : std::filesystem::directory_iterator(assetsFolder))
                {
                    if (file.path().extension() == ".obj")
                    {
                        std::string name = file.path().filename().string();
                        if (ImGui::Button(name.c_str()))
                        {
                            auto mesh = loadObjectMesh(assetsFolder + name, std::make_shared<Material>());
                            scene.addEntity(mesh);
                            scene.buildBVH();
                            render = true;
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::Text("Scene Objects");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::BeginChild("ObjectList");
            for (size_t i = 0; i < scene.getFiniteEntities().size(); ++i)
            {
                Entity* entity = scene.getFiniteEntities()[i];

                if (dynamic_cast<Light*>(entity))
                {
                    continue;
                }
                std::string name = entity->getName() + " " + std::to_string(i);
                if (ImGui::Selectable(name.c_str(), selectedEntity == entity))
                {
                    selectedEntity = entity;
                    selectedLight = nullptr;
                }
            }
            for (size_t i = 0; i < scene.getInfiniteEntities().size(); ++i)
            {
                Entity* entity = scene.getInfiniteEntities()[i];
                std::string name = entity->getName() + " " + std::to_string(i);
                if (ImGui::Selectable(name.c_str(), selectedEntity == entity))
                {
                    selectedEntity = entity;
                    selectedLight = nullptr;
                }
            }
            for (size_t i = 0; i < scene.getLights().size(); ++i)
            {
                Light* light = scene.getLights()[i];
                std::string name = "Light " + std::to_string(i);
                if (dynamic_cast<AreaLight*>(light))
                {
                    if (ImGui::Selectable(name.c_str(), selectedLight == light))
                    {
                        selectedLight = light;
                        selectedEntity = dynamic_cast<Entity*>(light);
                    }
                }
                else
                {
                    if (ImGui::Selectable(name.c_str(), selectedLight == light))
                    {
                        selectedLight = light;
                        selectedEntity = nullptr;
                    }
                }
            }
            ImGui::EndChild();
            ImGui::End();

            if (selectedEntity)
            {
                ImGui::SetNextWindowPos(ImVec2(1100, 10));
                ImGui::Begin("Entity Properties", nullptr);
                Transform& t = selectedEntity->editTransform();
                Material& mat = selectedEntity->editMaterial();
                ImGui::Text("Transform");
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                Plane* p = nullptr;
                if ((p = dynamic_cast<Plane*>(selectedEntity)))
                {
                    glm::vec3 pNormal = p->getNormal();
                    glm::vec3 pPos = p->getPosition();
                    ImGui::InputFloat("X", &pPos.x, 0.1f, 1.0f,"%.1f");
                    ImGui::InputFloat("Y", &pPos.y, 0.1f, 1.0f,"%.1f");
                    ImGui::InputFloat("Z", &pPos.z, 0.1f, 1.0f,"%.1f");
                    ImGui::DragFloat3("Normal", &pNormal.x, 0.1f, -1.0f, 1.0f, "%.1f");
                    p->setPosition(pPos);
                    p->setNormal(pNormal);
                    selectedEntity = p;
                }
                else
                {
                    ImGui::InputFloat("X", &t.position.x, 0.1f, 1.0f,"%.1f");
                    ImGui::InputFloat("Y", &t.position.y, 0.1f, 1.0f,"%.1f");
                    ImGui::InputFloat("Z", &t.position.z, 0.1f, 1.0f,"%.1f");
                    ImGui::SliderFloat3("Rotation", &t.rotation.x, 0.0f, 360, "%.0f");
                    ImGui::InputFloat("ScaleX", &t.scale.x, 0.01f, 1.0f, "%.2f");
                    ImGui::InputFloat("ScaleY", &t.scale.y, 0.01f, 1.0f, "%.2f");
                    ImGui::InputFloat("ScaleZ", &t.scale.z, 0.01f, 1.0f, "%.2f");
                }
                if (ImGui::Button("Apply Transform"))
                {
                    render = true;
                    scene.buildBVH();
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Material");
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::ColorEdit3("Colour", &mat.colour.x);
                ImGui::SliderFloat("Roughness", &mat.roughness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Metallic", &mat.metallic, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Reflectivity", &mat.reflectivity, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("IOR", &mat.iOR, 1.0f, 3.0f, "%.2f");
                ImGui::SliderFloat("Transparency", &mat.transparency, 0.0f, 1.0f, "%.2f");
                ImGui::ColorEdit3("Attenuation", &mat.attenuation.x);
                ImGui::ColorEdit3("Emissive", &mat.emissive.x);
                ImGui::SliderFloat("ClearCoat", &mat.clearcoat, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Clearcoat Roughness", &mat.clearcoatRoughness,
                                   0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Anisotropy", &mat.anisotropy, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat("Anisotropy Rotation", &mat.anisotropyRotation,
                                   -180.0f, 180.0f, "%.0f");
                ImGui::SliderFloat("Sheen", &mat.sheen, 0.0f, 1.0f, "%.2f");
                ImGui::ColorEdit3("Sheen Colour", &mat.sheenColour.x);
                ImGui::Spacing();
                if (ImGui::Button("Apply Material"))
                {
                    render = true;
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Text("Textures");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                for (const auto& preset : texturePresets)
                {
                    if (ImGui::Button(preset.name.c_str()))
                    {
                        mat.colourMap = Texture::loadTexture(preset.colourPath);
                        mat.roughnessMap = Texture::loadTexture(preset.roughnessPath);
                        mat.normalMap = Texture::loadTexture(preset.normalPath);
                        mat.metallicMap = Texture::loadTexture(preset.metallicPath);
                        mat.emissiveMap = Texture::loadTexture(preset.emissivePath);
                        mat.aoMap = Texture::loadTexture(preset.aoPath);
                        render = true;
                    }
                }
                ImGui::Spacing();
                ImGui::Spacing();
                if (ImGui::Button("Remove Texture")) {
                    mat.colourMap = nullptr;
                    mat.roughnessMap = nullptr;
                    mat.metallicMap = nullptr;
                    mat.normalMap = nullptr;
                    mat.aoMap = nullptr;
                    mat.emissiveMap = nullptr;
                    render = true;
                }
                ImGui::Spacing();
                ImGui::Spacing();
                if (mat.colourMap != nullptr)
                {
                    ImGui::SliderFloat("Colour Map Intensity",&mat.colourMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (mat.roughnessMap != nullptr)
                {
                    ImGui::SliderFloat("Roughness Map Intensity",&mat.roughnessMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (mat.metallicMap != nullptr)
                {
                    ImGui::SliderFloat("Metallic Map Intensity",&mat.metallicMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (mat.normalMap != nullptr)
                {
                    ImGui::SliderFloat("Normal Map Intensity",&mat.normalMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (mat.aoMap != nullptr)
                {
                    ImGui::SliderFloat("Ambient Occlusion Map Intensity",&mat.aoMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (mat.emissiveMap != nullptr)
                {
                    ImGui::SliderFloat("Emissive Map Intensity",&mat.emissiveMapIntensity,0.0f, 1.0f, "%.2f");
                }
                if (ImGui::Button("Apply Map Intensities")) {
                    render = true;
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (!dynamic_cast<Light*>(selectedEntity))
                {
                    if (ImGui::Button("Delete Object", ImVec2(150, 30)))
                    {
                        scene.removeEntity(selectedEntity);
                        scene.buildBVH();
                        selectedEntity = nullptr;
                        render = true;
                    }
                }
                ImGui::End();
            }
            if (selectedLight)
            {
                ImGui::SetNextWindowPos(ImVec2(1340, 750));
                ImGui::Begin("Light Properties", nullptr);
                glm::vec3 colour = selectedLight->getColour();
                float intensity = selectedLight->getIntensity();
                if (ImGui::ColorEdit3("Colour", &colour.x))
                {
                    selectedLight->setColour(colour);
                }
                if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f, "%.0f"))
                {
                    selectedLight->setIntensity(intensity);
                }

                if (auto pointLight = dynamic_cast<PointLight*>(selectedLight))
                {
                    float radius = pointLight->getRadius();
                    glm::vec3 position = pointLight->getPosition();
                    if (ImGui::SliderFloat("Radius", &radius, 0.01f, 1.0f, "%.2f"))
                    {
                        pointLight->setRadius(radius);
                    }
                    if (ImGui::DragFloat3("Position", &position.x))
                    {
                        pointLight->setPosition(position);
                    }
                }
                if (auto dirLight = dynamic_cast<DirectionalLight*>(selectedLight))
                {
                    static float angle = dirLight->getAngle();
                    static glm::vec3 direction = dirLight->getDirection();
                    if (ImGui::DragFloat("Angle", &angle, 0.01f, 0.05f,30.0f,"%.2f"))
                    {
                        dirLight->setAngle(angle);
                    }
                   if (ImGui::DragFloat("DirX", &direction.x, 0.1,-1,1))
                    {
                        dirLight->setDirection(direction);
                    }
                    if (ImGui::DragFloat("DirY", &direction.y, 0.1,-1,1))
                    {
                        dirLight->setDirection(direction);
                    }
                    if (ImGui::DragFloat("DirZ", &direction.z, 0.1,-1,1))
                    {
                        dirLight->setDirection(direction);
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::Button("Apply Light Properties"))
                {
                    render = true;
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::Button("Delete Light"))
                {
                    scene.removeLight(selectedLight);
                    scene.buildBVH();
                    selectedLight = nullptr;
                    selectedEntity = nullptr;
                    render = true;
                }

                ImGui::End();
            }
        }
        if (cameraMode && state == RUNNING)
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                camera.moveZ(cameraSpeed);
                render = true;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                camera.moveZ(-cameraSpeed);
                render = true;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                camera.moveX(-cameraSpeed);
                render = true;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                camera.moveX(cameraSpeed);
                render = true;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                camera.moveY(cameraSpeed);
                render = true;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                camera.moveY(-cameraSpeed);
                render = true;
            }
        }


        if (render)
        {
            if (!showCustomRenderSettings)
            {
                renderParams.primarySamples = currentQuality.pSamples;
                renderParams.reflectionSamples = currentQuality.rSamples;
                renderParams.shadowSamples = currentQuality.sSamples;
                renderParams.maxDepth = currentQuality.maxDepth;
            }

            if (g_camera)
            {
                g_camera->setAspect(static_cast<float>(currentWidth) / static_cast<float>(currentHeight),
                                    currentWidth, currentHeight);
            }
            pixels.resize(currentWidth * currentHeight);


            glBindTexture(GL_TEXTURE_2D, texture);
            if (!cancelRender)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, currentWidth, currentHeight,
                             0, GL_RGB, GL_FLOAT, nullptr);
            }

            renderScene(scene, camera, renderParams, pixels, currentWidth, currentHeight);

            if (!cancelRender)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, currentWidth, currentHeight,
                             0, GL_RGB, GL_FLOAT, pixels.data());
            }
            render = false;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
};
