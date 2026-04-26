//
// Created by Lochlan Harvey on 09/02/2026.
//

#pragma once
#include "../Light.h"
#include <glm/glm.hpp>

class PointLight : public Light
{
public:
    PointLight(const glm::vec3& position,
               const glm::vec3& colour,
               const float intensity,
               const float radius = 0.0f)
        : position(position), colour(colour),
          brightness(intensity), radius(radius)
    {
    }

    ~PointLight() override = default;

    PointLight(PointLight&&) = delete;

    PointLight& operator=(PointLight&&) = delete;

    [[nodiscard]] glm::vec3 getColour() override { return colour * brightness; }
    void setColour(const glm::vec3 newColour) override { colour = newColour; }

    [[nodiscard]] float getIntensity() override { return brightness; }
    void setIntensity(const float newIntensity) override { brightness = newIntensity; }

    [[nodiscard]] glm::vec3 getPosition() const { return position; }

    void setPosition(const glm::vec3 pos)
    {
        position = pos;
    }

    [[nodiscard]] float getRadius() const { return radius; }
    void setRadius(const float newRadius) { radius = newRadius; }

private:
    glm::vec3 position;
    glm::vec3 colour;
    float brightness;
    float radius;
};