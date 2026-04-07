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
        Cylinder(const float radius, const float height, const std::shared_ptr<Material>& mat)
            : Entity(mat), radius(radius), height(height) {
        }

        Cylinder(const Cylinder&) = delete;

        Cylinder& operator=(const Cylinder&) = delete;

        bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
            Ray localRay = rayToObjectSpace(ray);
            const glm::vec3 rO = localRay.origin;
            const glm::vec3 rD = localRay.direction;

            const float a = rD.x * rD.x + rD.z * rD.z;
            const float b = 2.0f * (rO.x * rD.x + rO.z * rD.z);
            const float c = rO.x * rO.x + rO.z * rO.z - radius * radius;
            const float discriminant = b * b - 4 * a * c;

            float t = INFINITY;
            if (discriminant >= 0 && glm::abs(a) > 1e-6f) {
                glm::vec3 normal;
                const float sqrtD = glm::sqrt(discriminant);
                const float t1 = (-b - sqrtD) / (2 * a);
                const float t2 = (-b + sqrtD) / (2 * a);


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
                    hit.point = localRay.positionAt(t);
                    glm::vec3 localPoint = hit.point;

                    float u = atan2(localPoint.z,localPoint.x) / (2 * M_PI);
                    if (u > 0) {
                        u += 1.0f;
                    }

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

                    hit.tangent = glm::vec3(-std::sin(u * 2 * M_PI),0, std::cos(u * 2 * M_PI));
                    hit.bitangent = glm::cross(hit.normal, hit.tangent);

                    objectIntersectionToWorldSpace(hit);
                    hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                    hit.setFrontSurface(ray,hit.normal);

                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const glm::mat4 matrix = transform.getMatrix();

            const glm::vec3 localMin(-radius, -height * 0.5f, - radius);
            const glm::vec3 localMax(radius, height * 0.5f, radius);


             glm::vec3 vertices[8];
            for (int i = 0; i < 8; i++) {
                glm::vec3 vertex(
                    (i & 1) ? localMax.x : localMin.y,
                    (i & 2) ? localMax.y : localMin.y,
                    (i & 4) ? localMax.z : localMin.z);
                vertices[i] = glm::vec3(matrix * glm::vec4(vertex, 1.0f));
            }
            glm::vec3 worldMin = vertices[0];
            glm::vec3 worldMax = vertices[0];
            for (int i = 1; i < 8; i++) {
                worldMin = glm::min(worldMin, vertices[i]);
                worldMax = glm::max(worldMax, vertices[i]);
            }
            const auto bB = BoundingBox(worldMin, worldMax);
            return bB;
        }

    private:
        float radius;
        float height;
};
