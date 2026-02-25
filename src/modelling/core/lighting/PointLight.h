//
// Created by Lochlan Harvey on 09/02/2026.
//

#pragma once
#include "Light.h"
#include <glm/glm.hpp>

class PointLight : public Light {
    public:
        PointLight(const glm::vec3& position,
                   const glm::vec3& colour,
                   const float intensity,
                   const float radius = 0.0f)
        : position(position), colour(colour),
        brightness(intensity), radius(radius) {}

        ~PointLight() override = default;

        PointLight(PointLight&&) = delete;

        PointLight& operator=(PointLight&&) = delete;

        [[nodiscard]] glm::vec3 getColour() const override {
            return colour * brightness;
        }

        glm::vec3 getPosition() const {return position;}
        float getBrightness() const {return brightness;}
        float getRadius() const { return  radius;}

    private:
        glm::vec3 position;
        glm::vec3 colour;
        float brightness;
        float radius;
};