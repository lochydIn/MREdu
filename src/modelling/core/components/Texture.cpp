//
// Created by Lochlan Harvey on 17/03/2026.
//
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

#include <iostream>
#include <ostream>

#include "glm/ext/scalar_common.hpp"

Texture::Texture(const std::string& filename) {
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);

    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        width = height = 1;
        pixels.emplace_back(1.0f, 0.0f, 1.0f);
        return;
    }

    pixels.resize(width * height);
    for (int i = 0; i < width * height; i++) {
        pixels[i] = glm::vec3(
            data[i*3 + 0] / 255.0f,
            data[i*3 + 1] / 255.0f,
            data[i*3 + 2] / 255.0f
        );
    }
    stbi_image_free(data);

}

Texture::~Texture()= default;

glm::vec3 Texture::sample(float u, float v) const {
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);

    int x = int(u * width);
    int y = int(v * height);

    // Clamp to valid range
    x = glm::min(x, width - 1);
    y = glm::min(y, height - 1);


    return pixels[y * width + x];
}
