//
// Created by Lochlan Harvey on 26/01/2026.
//
# pragma once
#include "glm/vec3.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;


    explicit Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin(origin), direction(direction) {}

    virtual ~Ray() = default;

    [[nodiscard]] glm::vec3 positionAt (float t) const {
        return origin + direction * t;
    }
};
