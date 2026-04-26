//
// Created by Lochlan Harvey on 10/02/2026.
//

#pragma once
#include "Light.h"
#include "LightSample.h"
#include "../Entity.h"
#include "../../../rendering/structs/Ray.h"
#include "../../../rendering/structs/Intersection.h"

class AreaLight : public Light, public Entity
{
public:
    AreaLight(const glm::vec3 colour, const float intensity)
        : colour(colour), intensity(intensity)
    {
    }

    ~AreaLight() override = default;

    [[nodiscard]] virtual LightSample sample(const glm::vec3& hitPoint,
                                             float r1, float r2) const = 0;

    void setColour(const glm::vec3 newColour) override
    {
        colour = newColour;
        radiance = colour * intensity / area;
        updateMaterial();
    }

    glm::vec3 getColour() override { return colour; }

    void setIntensity(const float newIntensity) override
    {
        intensity = newIntensity;
        radiance = colour * intensity / area;
        updateMaterial();
    }

    float getIntensity() override { return intensity; }

    [[nodiscard]] glm::vec3 getRadiance() const { return radiance; }

    [[nodiscard]] virtual float getArea() const { return area; }

    [[nodiscard]] bool isLight() const override { return true; }

protected:
    void updateMaterial() const
    {
        if (material)
        {
            auto& mat = const_cast<Material&>(*material);
            mat.emissive = radiance * 10.0f;
            mat.colour = colour;
        }
    }

    float area;
    glm::vec3 colour;
    float intensity;
    glm::vec3 radiance;
};