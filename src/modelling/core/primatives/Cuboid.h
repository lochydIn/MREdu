//
// Created by Lochlan Harvey on 24/02/2026.
//

#pragma once
#include "../Entity.h"
#include "../components/Transform.h"

class Cuboid : public Entity {
    public:
        Cuboid(const std::shared_ptr<Material>& material) : Entity(material) {
            min = glm::vec3(-0.5f);
            max = glm::vec3(0.5f);
        }

        Cuboid(const Cuboid&) = delete;

        Cuboid& operator = (const Cuboid&) = delete;

        bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
            Ray localRay = rayToObjectSpace(ray);

            float tEnter = -INFINITY;
            float tExit = INFINITY;

            for (int axis = 0; axis < 3; axis++) {
                const float origin = localRay.origin[axis];
                const float direction = localRay.direction[axis];

                const float minBound = min[axis];
                const float maxBound = max[axis];

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
            hit.point = localRay.positionAt(t);

            glm::vec3 localNormal;
            glm::vec3 localPoint = hit.point;
            glm::vec2 uv;
            glm::vec3 size = max - min;

            float epsilon = 1e-6f;

            if (std::abs(localPoint.x - min.x) < epsilon) {
                localNormal = glm::vec3(-1,0,0);
                uv = glm::vec2(
                    (localPoint.z - min.z) / size.z,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(0, 0, 1);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.x - max.x) < epsilon) {
                localNormal = glm::vec3(1,0,0);
                uv = glm::vec2(
                    1.0f - (localPoint.z - min.z) / size.z,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(0, 0, 1);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.y - min.y) < epsilon) {
                localNormal = glm::vec3(0,-1,0);
                uv = glm::vec2(
                    (localPoint.x - min.x) / size.x,
                    (localPoint.z - min.z) / size.z);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 0, 1);
            } else if (std::abs(localPoint.y - max.y) < epsilon) {
                localNormal = glm::vec3(0,1,0);
                uv = glm::vec2(
                    1.0f - (localPoint.x - min.x) / size.x,
                    (localPoint.z - min.z) / size.z);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 0, 1);
            } else if (std::abs(localPoint.z - min.z) < epsilon) {
                localNormal = glm::vec3(0,0,-1);
                uv = glm::vec2(
                    (localPoint.x - min.x) / size.x,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 1, 0);
            } else if (std::abs(localPoint.z - max.z) < epsilon) {
                localNormal = glm::vec3(0,0,1);
                uv = glm::vec2(
                    1.0f - (localPoint.x - min.x) / size.x,
                    (localPoint.y - min.y) / size.y);
                hit.tangent = glm::vec3(1, 0, 0);
                hit.bitangent = glm::vec3(0, 1, 0);
            }
            hit.normal = localNormal;
            hit.uv = uv;

            hit.tangent = glm::normalize(hit.tangent - glm::dot(hit.tangent, hit.normal) * hit.normal);
            hit.bitangent = glm::cross(hit.normal, hit.tangent);

            objectIntersectionToWorldSpace(hit);
            hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
            hit.setFrontSurface(ray,hit.normal);
            return true;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const glm::mat4 matrix = transform.getMatrix();

            glm::vec3 vertices[8];
            for (int i = 0; i < 8; i++) {
                glm::vec3 localVertex(
                    (i & 1) ? max.x : min.x, (i & 2) ? max.y : min.y, (i & 4) ? max.z : min.z);
                vertices[i] = glm::vec3(matrix * glm::vec4(localVertex,1.0f));
            }
            glm::vec3 worldMin = vertices[0];
            glm::vec3 worldMax = vertices[0];
            for (int i = 1; i < 8; i++) {
                worldMin = glm::min(worldMin, vertices[i]);
                worldMax = glm::max(worldMax, vertices[i]);
            }

            auto const bB = BoundingBox(worldMin, worldMax);
            return bB;
        }

    private:
        glm::vec3 min;
        glm::vec3 max;

};