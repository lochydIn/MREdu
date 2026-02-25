//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "Light.h"
#include "glm/vec3.hpp"

class DirectionalLight : public Light {
    public:

        DirectionalLight(const glm::vec3& direction,
            const glm::vec3& colour, float intensity, float angle)
        : direction (glm::normalize(direction)), colour(colour), brightness(intensity), angle(angle) {}

        ~DirectionalLight() override = default;
        DirectionalLight(DirectionalLight&&) = delete;
        DirectionalLight& operator = (DirectionalLight&&) = delete;

        [[nodiscard]] glm::vec3 getDirection() const {
            return -direction;
        }
        [[nodiscard]] glm::vec3 getColour() const override {
            return colour * brightness;
        }

        [[nodiscard]] float getAngle() const {
            return angle;
        }

        static float getDistance() {
            return FLT_MAX; //Light is infinitely far away.
        }



    private:
        glm::vec3 direction;
        glm::vec3 colour;
        float brightness;
        float angle;
};
