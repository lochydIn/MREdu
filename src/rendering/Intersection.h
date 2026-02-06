//
// Created by Lochlan Harvey on 02/02/2026.
//

#pragma once
#include <glm/glm.hpp>

#include "Ray.h"

struct Intersection
{
    glm::vec3 point; //Hit pos.
    glm::vec3 normal; // Surface Normal
    bool frontSurface;
    float distance = -1.0f; // The distance along the ray that the hit occurs.

    void setFrontSurface(const Ray& ray, const glm::vec3& outwardNormal)
    {
        frontSurface = glm::dot(ray.direction, outwardNormal) < 0;
        if (frontSurface)
        {
            normal = outwardNormal;
        } else
        {
            normal = -outwardNormal;
        }
    }

};
