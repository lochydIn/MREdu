//
// Created by Lochlan Harvey on 08/02/2026.
//

#pragma once
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"
#include "../Entity.h"
#include "../../../rendering/Intersection.h"
#include "../../../rendering/Ray.h"

class Plane : public Entity {
    public:
    Plane(const glm::vec3& position, const glm::vec3 normal, std::shared_ptr<Material> mat) :
    Entity(std::move(mat)), position(position),normal(normal) {}

    bool intersect(const Ray& ray, Intersection& hit) const override {
        float denom = glm::dot(normal, ray.direction);

        if (fabs(denom) > 0.0001f) { // If intersecting ray is not parallel.
            float t = glm::dot(position - ray.origin, normal) / denom;
            if (t > 0.001f && t < hit.distance) {
                hit.point = ray.origin + t * ray.direction;
                hit.normal = normal;
                hit.distance = t;
                hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                return true;
            }
        }
        return false;
    }

private:
    glm::vec3 position;
    glm::vec3 normal;
};
