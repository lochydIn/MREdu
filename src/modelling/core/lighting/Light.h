//
// Created by Lochlan Harvey on 09/02/2026.
//

#pragma once
#include <glm/glm.hpp>
class Light {
public:

    Light() = default;
    virtual ~Light() = default;
    [[nodiscard]] virtual glm::vec3 getColour() = 0;
    virtual void setColour(glm::vec3 colour) = 0;
    [[nodiscard]] virtual float getIntensity() = 0;
    virtual void setIntensity(float intensity) = 0;

    Light(const Light&) = delete;
    Light& operator=(const Light&) = delete;
};