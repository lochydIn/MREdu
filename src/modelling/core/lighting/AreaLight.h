//
// Created by Lochlan Harvey on 10/02/2026.
//

#pragma once
#include "Light.h"
#include "LightSample.h"
#include "../Entity.h"
#include "../../../rendering/structs/Ray.h"
#include "../../../rendering/structs/Intersection.h"

class AreaLight : public Light , public Entity {
    public:
        AreaLight(const glm::vec3 colour, const float intensity)
            : colour(colour), intensity(intensity) {}

        [[nodiscard]] virtual LightSample sample(const glm::vec3& hitPoint,
            float r1, float r2) const = 0;

        [[nodiscard]] virtual float getArea() const = 0;

        [[nodiscard]] virtual glm::vec3 getRadiance() const { return radiance; }

        [[nodiscard]] bool isLight() const override { return true; }

    protected:
        glm::vec3 colour;
        glm::vec3 intensity;
        glm::vec3 radiance;
};
