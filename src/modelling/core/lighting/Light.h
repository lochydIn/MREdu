//
// Created by Lochlan Harvey on 09/02/2026.
//

#pragma once
#include <glm/glm.hpp>
class Light
{
public:

    Light() = default;
    virtual ~Light() = default;

    [[nodiscard]] virtual glm::vec3 getDirection(const glm::vec3& point) const = 0;
    [[nodiscard]] virtual glm::vec3 getColour() const = 0;
    [[nodiscard]] virtual float getIntensity() const = 0;
    [[nodiscard]] virtual float getDistance(const glm::vec3& point) const = 0;

    [[nodiscard]] virtual glm::vec3 samplePosition() const { return glm::vec3(0);}

    Light(const Light&) = delete;
    Light& operator=(const Light&) = delete;


};