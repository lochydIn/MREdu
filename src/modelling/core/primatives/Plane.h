//
// Created by Lochlan Harvey on 08/02/2026.
//

#pragma once
#include "glm/geometric.hpp"
#include "glm/vec3.hpp"
#include "../Entity.h"


class Plane : public Entity {
    public:
    Plane(const glm::vec3& position, const glm::vec3 normal, std::shared_ptr<Material> mat) :
    Entity(std::move(mat)), position(position),normal(normal) {}

    bool intersect(const Ray& ray, Intersection& intersection, float tMin, float tMax) const override {
        // If intersecting ray is not parallel.
        if (const float denom = glm::dot(normal, ray.direction); glm::abs(glm::dot(normal, ray.direction)) > 0.0001f) {

            float t = glm::dot(position - ray.origin, normal) / denom;

            if (t > tMin && t < tMax) {
                intersection.point = ray.origin + t * ray.direction;
                intersection.normal = (denom < 0.0f) ? normal : -normal;
                glm::vec3 local = intersection.point - position;
                float tileSize = 1.0f;
                intersection.uv.x = glm::fract(local.x / tileSize + 0.5f);
                intersection.uv.y = glm::fract(local.y / tileSize + 0.5f);
                intersection.distance = t;
                intersection.entity = const_cast<Plane*>(this);
                intersection.setFrontSurface(ray,intersection.normal);
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override {
        const auto bB = BoundingBox(glm::vec3(-LARGE_BOUND), glm::vec3(LARGE_BOUND));
        return bB;
    }

private:
    static constexpr float LARGE_BOUND = 1e30f;
    glm::vec3 position;
    glm::vec3 normal;
};
