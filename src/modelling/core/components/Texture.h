//
// Created by Lochlan Harvey on 16/03/2026.
//

#pragma once
#include <string>
#include <vector>
#include "glm/vec3.hpp"

class Texture {
    public:
        Texture(const std::string& filename);
        ~Texture();

        [[nodiscard]] glm::vec3 sample(float u, float v) const;
        [[nodiscard]] int getWidth() const { return width; }
        [[nodiscard]] int getHeight() const { return height; }

    private:
        int width, height, channels;
        std::vector<glm::vec3> pixels;

};
