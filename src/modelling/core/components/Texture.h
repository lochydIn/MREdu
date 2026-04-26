//
// Created by Lochlan Harvey on 16/03/2026.
//

#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "glm/vec3.hpp"

class Texture
{
public:
    Texture(const std::string& filename);

    ~Texture();

    [[nodiscard]] glm::vec3 sample(float u, float v) const;
    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }

    static std::shared_ptr<Texture> loadTexture(const std::string& filename)
    {
        if (filename.empty())
        {
            return nullptr;
        }
        return std::make_shared<Texture>(filename);
    }

private:
    int width, height, channels;
    std::vector<glm::vec3> pixels;
};

struct TexturePreset
{
    std::string name;
    std::string colourPath;
    std::string roughnessPath;
    std::string normalPath;
    std::string emissivePath;
    std::string metallicPath;
    std::string aoPath;
};

