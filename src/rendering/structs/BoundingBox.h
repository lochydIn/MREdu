//
// Created by Lochlan Harvey on 19/02/2026.
//

#pragma once
#include "glm/vec3.hpp"
#include "glm/gtx/extended_min_max.hpp"
#include "Ray.h"

struct BoundingBox
{
    glm::vec3 min; // Bottom left coord
    glm::vec3 max; // Top right coord.

    // 3 constructors
    BoundingBox() : min(1e30f), max(-1e30f)
    {
    } // Empty Box.
    BoundingBox(const glm::vec3& p) : min(p), max(p)
    {
    } // Point-sized box.
    BoundingBox(const glm::vec3& min, const glm::vec3& max) : min(min), max(max)
    {
    } //Full box.


    void expand(const glm::vec3& p)
    {
        min = glm::min(min, p);
        max = glm::max(max, p);
    }

    void expand(const BoundingBox& box)
    {
        min = glm::min(min, box.min);
        max = glm::max(max, box.max);
    }


    // Does the ray intersect the box. (Slab Method)
    bool intersect(const Ray& ray, float& tMin, float& tMax) const
    {
        // Reciprocal Direction
        glm::vec3 invDir = 1.0f / ray.direction;

        glm::vec3 t1 = (min - ray.origin) * invDir; //Time to hit near plane (x,y,z)
        glm::vec3 t2 = (max - ray.origin) * invDir; // Time to hit far plane. (x,y,z)

        glm::vec3 tNear = glm::min(t1, t2); // (which x y z is the entry point)
        glm::vec3 tFar = glm::max(t1, t2); // exit Point

        tMin = glm::max(glm::max(tNear.x, tNear.y), tNear.z); // Which is the last entry, x  y or z.
        tMax = glm::min(glm::min(tFar.x, tFar.y), tFar.z); // Which is the first exit, x y or z.

        return tMin <= tMax && tMax > 0;
        // if entry time before exit time (did it go through) && is box in front of ray.
    }

    // Which is the boxes longest dimension ? - for splitting.
    [[nodiscard]] int maxDimension() const
    {
        glm::vec3 extent = max - min;
        if (extent.x > extent.y && extent.x > extent.z) { return 0; }
        if (extent.y > extent.x) { return 1; }
        return 2;
    }
};