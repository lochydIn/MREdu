//
// Created by Lochlan Harvey on 09/02/2026.
//

#pragma once
#include "Light.h"
#include <glm/glm.hpp>

class PointLight : public Light {
    public:
        PointLight(const glm::vec3& position, const glm::vec3& colour,
            const float intensity, const float radius = 1.0f)
        : position(position), colour(colour),
        intensity(intensity), radius(radius) {}

        ~PointLight() override = default;

        PointLight(PointLight&&) = delete;

        PointLight& operator=(PointLight&&) = delete;

        [[nodiscard]] glm::vec3 getDirection(const glm::vec3& point) const override {
            return glm::normalize(position - point); // Vector from point to light.
        }
        [[nodiscard]] glm::vec3 getColour() const override {
            return colour;
        }
        [[nodiscard]] float getIntensity() const override {
            return intensity;
        }
        [[nodiscard]] float getDistance(const glm::vec3& point) const override {
            return glm::length(position - point);
        }

    private:
        glm::vec3 position;
        glm::vec3 colour;
        float intensity;
        float radius;
};