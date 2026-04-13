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

    [[nodiscard]] std::string getName() const override {
        return "Plane";
    }

    [[nodiscard]] glm::vec3 getNormal() const {
        return normal;
    }
    void setNormal(const glm::vec3& newNormal) {
        normal = newNormal;
    }

    [[nodiscard]] glm::vec3 getPosition() const {
        return position;
    }

    void setPosition(const glm::vec3& newPosition) {
        position = newPosition;
    }

    bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
        // If intersecting ray is not parallel.
        if (const float denom = glm::dot(normal, ray.direction); glm::abs(glm::dot(normal, ray.direction)) > 0.0001f) {

            float t = glm::dot(position - ray.origin, normal) / denom;

            if (t > tMin && t < tMax) {
                hit.point = ray.origin + t * ray.direction;
                hit.normal = (denom < 0.0f) ? normal : -normal;
                hit.setFrontSurface(ray,hit.normal);
                hit.tangent = glm::vec3(1,0,0);
                hit.bitangent = glm::vec3(0,0,1);
                hit.bitangent = glm::cross(hit.normal, hit.tangent);
                const glm::vec3 local = hit.point - position;
                constexpr float tileSize = 1.0f;
                hit.uv.x = glm::fract(local.x / tileSize + 0.5f);
                hit.uv.y = glm::fract(local.y / tileSize + 0.5f);
                hit.distance = t;
                hit.entity = const_cast<Plane*>(this);
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
