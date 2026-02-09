//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "Light.h"
#include "glm/vec3.hpp"

class DirectionalLight : public Light {
public:

    DirectionalLight(const glm::vec3& direction,
        const glm::vec3& colour, float intensity)
    : direction (glm::normalize(direction)), colour(colour), intensity(intensity) {}

    ~DirectionalLight() override = default;

    DirectionalLight(DirectionalLight&&) = delete;

    DirectionalLight& operator = (DirectionalLight&&) = delete;

    [[nodiscard]] glm::vec3 getDirection(const glm::vec3& point) const override {
        return -direction;
    }
    [[nodiscard]] glm::vec3 getColour() const override {
        return colour;
    }
    [[nodiscard]] float getIntensity() const override {
        return intensity;
    }

    [[nodiscard]] float getDistance(const glm::vec3& point) const override {
        return FLT_MAX; //Light is infinitely far away.
    }

private:
    glm::vec3 direction;
    glm::vec3 colour;
    float intensity;
};
