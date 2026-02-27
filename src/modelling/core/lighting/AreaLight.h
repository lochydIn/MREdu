//
// Created by Lochlan Harvey on 10/02/2026.
//

#pragma once
#include "Light.h"
#include "LightSample.h"

class AreaLight : public Light , public Entity {
    public:
        AreaLight(const glm::vec3 colour, const float intensity)
            : colour(colour), radiance(colour * intensity) {}

        [[nodiscard]] virtual LightSample sample(const glm::vec3& hitPoint,
            float r1, float r2) const = 0;

        [[nodiscard]] virtual float getArea() const = 0;

    protected:
        glm::vec3 colour;
        glm::vec3 radiance;
};
