//
// Created by Lochlan Harvey on 24/02/2026.
//

#pragma once
#include "../Entity.h"
#include "../components/Transform.h"

class Cuboid : public Entity {
    public:
        Cuboid(const glm::vec3& position, const glm::vec3& min, const glm::vec3& max,
            const std::shared_ptr<Material>& material) : Entity(material), position(position), min(min),
            max(max) {}

        Cuboid(const Cuboid&) = delete;

        Cuboid& operator = (const Cuboid&) = delete;

        bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
            glm::vec3 boxMin = position + min;
            glm::vec3 boxMax = position + max;

            float tEnter = -INFINITY;
            float tExit = INFINITY;

            for (int axis = 0; axis < 3; axis++) {
                const float origin = ray.origin[axis];
                const float direction = ray.direction[axis];

                const float minBound = boxMin[axis];
                const float maxBound = boxMax[axis];

                if (std::abs(direction) < 1e-8f) {
                    if (origin < minBound || origin > maxBound) {
                        return false;
                    }
                    continue;
                }
                float t1 = (minBound - origin) / direction;
                float t2 = (maxBound - origin) / direction;
                float tNear = std::min(t1, t2);
                float tFar = std::max(t1, t2);

                tEnter = std::max(tEnter, tNear);
                tExit = std::min(tExit, tFar);

                if (tEnter > tExit || tExit < tNear) {
                    return false;
                }
            }

            float t = tEnter;
            if (t < tMin) t = tExit;
            if (t > tMax || t < tMin) return false;

            hit.distance = t;
            hit.point = ray.positionAt(t);
            hit.entity = const_cast<Cuboid*>(this);


            glm::vec3 localPoint = hit.point - position;
            glm::vec3 norm(0);
            hit.normal = glm::normalize(norm);
            glm::vec2 uv;
            glm::vec3 size = max - min;

            float epsilon = 1e-6f;

            if (std::abs(localPoint.x - min.x) < epsilon) {
                norm = glm::vec3(-1,0,0);
                uv = glm::vec2(
                    (localPoint.z - min.z) / size.z,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(0, 0, 1);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.x - max.x) < epsilon) {
                norm = glm::vec3(1,0,0);
                uv = glm::vec2(
                    1.0f - (localPoint.z - min.z) / size.z,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(0, 0, 1);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.y - min.y) < epsilon) {
                norm = glm::vec3(0,-1,0);
                uv = glm::vec2(
                    (localPoint.x - min.x) / size.x,
                    (localPoint.z - min.z) / size.z);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 0, 1);
            } else if (std::abs(localPoint.y - max.y) < epsilon) {
                norm = glm::vec3(0,1,0);
                uv = glm::vec2(
                    1.0f - (localPoint.x - min.x) / size.x,
                    (localPoint.z - min.z) / size.z);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 0, 1);
            } else if (std::abs(localPoint.z - min.z) < epsilon) {
                norm = glm::vec3(0,0,-1);
                uv = glm::vec2(
                    (localPoint.x - min.x) / size.x,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.z - max.z) < epsilon) {
                norm = glm::vec3(0,0,1);
                uv = glm::vec2(
                    1.0f - (localPoint.x - min.x) / size.x,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 1, 0);
            }
            hit.setFrontSurface(ray, hit.normal);
            hit.uv = uv;
            hit.tangent = glm::normalize(hit.tangent - glm::dot(hit.tangent, hit.normal) * hit.normal);
            hit.bitangent = glm::cross(hit.normal, hit.tangent);

            return true;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            auto const bB = BoundingBox(position + min, position + max);
            return bB;
        }

    private:
        Transform transform;
        glm::vec3 position;
        glm::vec3 min;
        glm::vec3 max;

};