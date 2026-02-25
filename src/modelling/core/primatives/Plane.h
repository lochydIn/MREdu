//
// Created by Lochlan Harvey on 08/02/2026.
//

#pragma once
#include <iostream>

#include "glm/geometric.hpp"
#include "glm/vec3.hpp"
#include "../Entity.h"
#include "../../../rendering/structs/Intersection.h"
#include "../../../rendering/structs/Ray.h"

class Plane : public Entity {
    public:
    Plane(const glm::vec3& position, const glm::vec3 normal, std::shared_ptr<Material> mat) :
    Entity(std::move(mat)), position(position),normal(normal) {}

    bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
        // If intersecting ray is not parallel.
        if (const float denom = glm::dot(normal, ray.direction); glm::abs(glm::dot(normal, ray.direction)) > 0.0001f) {

            float t = glm::dot(position - ray.origin, normal) / denom;

            if (t > tMin && t < tMax) {
                hit.point = ray.origin + t * ray.direction;
                hit.normal = (denom < 0.0f) ? normal : -normal;
                hit.distance = t;
                hit.entity = const_cast<Plane*>(this);
                hit.setFrontSurface(ray,hit.normal);
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override
    {
        return BoundingBox(glm::vec3(-LARGE_BOUND), glm::vec3(LARGE_BOUND));
    }

private:
    static constexpr float LARGE_BOUND = 1e30f;
    glm::vec3 position;
    glm::vec3 normal;
};
