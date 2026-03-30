//
// Created by Lochlan Harvey on 08/02/2026.
//

#pragma once
#include <cmath>

#include "glm/vec3.hpp"
#include "../components/Transform.h"
#include "../Entity.h"

class Cylinder : public Entity {
    public:
        Cylinder(const glm::vec3 position, const float radius, const float height, const std::shared_ptr<Material>& mat)
            : Entity(mat), position(position), radius(radius), height(height) {}

        Cylinder(const Cylinder&) = delete;

        Cylinder& operator=(const Cylinder&) = delete;

        bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
            const glm::vec3 rO = ray.origin - position;
            const glm::vec3 rD = ray.direction;

            const float a = rD.x * rD.x + rD.z * rD.z;
            const float b = 2.0f * (rO.x * rD.x + rO.z * rD.z);
            const float c = rO.x * rO.x + rO.z * rO.z - radius * radius;
            const float discriminant = b * b - 4 * a * c;

            float t = INFINITY;

            if (discriminant >= 0 && glm::abs(a) > 1e-6f) {
                const float sqrtD = glm::sqrt(discriminant);
                const float t1 = (-b - sqrtD) / (2 * a);
                const float t2 = (-b + sqrtD) / (2 * a);
                glm::vec3 normal;

                float tBody = INFINITY;
                if (t1 > tMin) {
                    float y = rO.y + t1 * rD.y;
                    if (glm::abs(y) < height * 0.5f) {
                        tBody = t1;
                        normal = glm::normalize(glm::vec3(rO.x + t1 * rD.x, 0, rO.z + t1 * rD.z));

                    }
                }
                if (t2 > tMin && t2 < tBody) {
                    float y = rO.y + t2 * rD.y;
                    if (glm::abs(y) < height * 0.5f) {
                        tBody = t2;
                        normal = glm::normalize(glm::vec3(rO.x + t2 * rD.x, 0, rO.z + t2 * rD.z));

                    }
                }
                if (tBody < t) {
                    t = tBody;
                }

                for (int cap = -1; cap <= 1; cap+=2) {
                    const float capY = cap * height * 0.5f;
                    float const tCap = (capY - rO.y) / rD.y;

                    if (tCap > tMin && tCap < t) {
                        const glm::vec3 point = rO + rD * tCap;
                        float dist2 = point.x * point.x + point.z * point.z;

                        if (dist2 <= radius * radius) {
                            t = tCap;
                            normal = glm::vec3(0,cap,0);
                        }
                    }
                }
                if (t < INFINITY && t < tMax) {
                    hit.distance = t;
                    hit.normal = normal;
                    hit.point = ray.positionAt(t);
                    glm::vec3 localPoint = hit.point - position;
                    float u = atan2(localPoint.z,localPoint.x) / (2 * M_PI);
                    if (u > 0) {
                        u += 1.0f;
                    }
                    hit.tangent = glm::vec3(-sin(u * 2 * M_PI),0, cos(u * 2 * M_PI));
                    hit.bitangent = glm::cross(hit.normal, hit.tangent);

                    if (glm::abs(hit.normal.y) < 0.1f) {
                        if (u < 0.0f) {
                            u += 1.0f;
                        }
                        float v = (localPoint.y + height * 0.5f) / height;
                        hit.uv = glm::vec2(u, v);
                    } else {
                        const float r = glm::length(glm::vec2(localPoint.x, localPoint.z)) / radius;
                        const float angle = atan2(localPoint.z,localPoint.x);

                        const float u = r * std::cos(angle) * 0.5f + 0.5f;
                        const float v = r * std::sin(angle) * 0.5f + 0.5f;
                        hit.uv = glm::vec2(u, v);
                    }
                    hit.entity = const_cast<Cylinder*>(this);
                    hit.setFrontSurface(ray, hit.normal);
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const auto bB = BoundingBox(position - glm::vec3(radius, height * 0.5f, radius),
                position + glm::vec3(radius, height * 0.5f, radius));
            return bB;
        }

    private:
        glm::vec3 position;
        float radius;
        float height;
        Transform transform;
};
