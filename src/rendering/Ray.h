//
// Created by Lochlan Harvey on 26/01/2026.
//
# pragma once
#include "glm/vec3.hpp"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    explicit Ray(const glm::vec3& m_origin, const glm::vec3& m_direction)
        : origin(m_origin), direction(m_direction) {}

    virtual ~Ray() = default;

    [[nodiscard]] glm::vec3 positionAt (float t) const {
        return origin + direction * t;
    }
};
