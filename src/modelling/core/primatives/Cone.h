//
// Created by Lochlan Harvey on 25/02/2026.
//

#pragma once
#include <cmath>
#include <memory>
#include "../Entity.h"
#include "../components/Material.h"
#include "../components/Transform.h"

class Cone : public Entity {
    public:
        Cone(const float height, const float radius, const std::shared_ptr<Material>& mat)
            : Entity(mat), radius(radius), height(height * 2) {}

        Cone(const Cone&) = delete;

        Cone& operator=(const Cone&) = delete;

        bool intersect (const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
            Ray localRay = rayToObjectSpace(ray);
            glm::vec3 ro = localRay.origin;
            glm::vec3 rd = localRay.direction;

            // Cone equation: radius at height y = radius * (1 - y/(height/2))
            // Solve quadratic: a*t^2 + b*t + c = 0
            float k = radius / (height * 0.5f); // taper factor

            float a = rd.x * rd.x + rd.z * rd.z - k * k * rd.y * rd.y;
            float b = 2.0f * (ro.x * rd.x + ro.z * rd.z - k * k * ro.y * rd.y);
            float c = ro.x * ro.x + ro.z * ro.z - k * k * ro.y * ro.y;

            float discriminant = b * b - 4 * a * c;

            float t = INFINITY;
            glm::vec3 normal;

            // Check cone body
            if (discriminant >= 0 && std::abs(a) > 1e-6f) {
                float sqrtD = std::sqrt(discriminant);
                float t1 = (-b - sqrtD) / (2 * a);
                float t2 = (-b + sqrtD) / (2 * a);



                // If the point of intersection is not above the middle.

                if (t1 > tMin) {
                    float y = ro.y + t1 * rd.y;
                    if (y >= -height * 0.5f && y <= height * 0.5f) {
                        if (y <= 0.0f) {
                            t = t1;
                            glm::vec3 point = ro + rd * t1;
                            auto normalDir = glm::vec3(point.x, -k * k * point.y, point.z);
                            normal = glm::normalize(normalDir);
                        }
                    }
                }


                if (t2 > tMin && t2 < t) {
                    float y = ro.y + t2 * rd.y;
                    if (y >= -height * 0.5f && y <= height * 0.5f) {
                        if (y <= 0.0f) {
                            t = t2;
                            glm::vec3 point = ro + rd * t2;
                            glm::vec3 normalDir = glm::vec3(point.x, -k * k * point.y, point.z);
                            normal = glm::normalize(normalDir);
                        }
                    }
                }
            }

            // Check base cap (only bottom)
            float capY = -height * 0.5f;
            float tCap = (capY - ro.y) / rd.y;

            if (tCap > tMin && tCap < t) {
                glm::vec3 point = ro + rd * tCap;
                float dist2 = point.x * point.x + point.z * point.z;
                float maxR = radius * (1 - (point.y + height * 0.5f) / height); // Tapered radius at base

                if (dist2 <= maxR * maxR) {
                    t = tCap;
                    normal = glm::vec3(0, -1, 0);
                }
            }

            if (t < INFINITY && t < tMax) {
                hit.distance = t;
                hit.point = ray.positionAt(t);
                hit.normal = normal;
                glm::vec3 localPoint = hit.point;
                hit.setFrontSurface(ray, hit.normal);

                float u = atan2(localPoint.z, localPoint.x) / (2.0f * M_PI);
                if (u > 0) {
                    u += 1.0f;
                }

                hit.tangent = glm::vec3(-sin(u * 2 * M_PI),0,cos(u * 2 * M_PI));
                hit.bitangent = glm::cross(hit.normal, hit.tangent);

                float v = (localPoint.y + height * 0.5f) / height;

                if (glm::abs(localPoint.y) < height * 0.5f && localPoint.y > -height * 0.5f) {
                    hit.uv = glm::vec2(u, v);
                } else {
                    float r = glm::length(glm::vec2(localPoint.x, localPoint.z)) / radius;
                    float angle = atan2(localPoint.z, localPoint.x);
                    hit.uv = glm::vec2(r * glm::cos(angle), r * glm::sin(angle)) * 0.5f + 0.5f;
                }
                objectIntersectionToWorldSpace(hit);
                hit.entity = const_cast<Cone*>(this);
                return true;
            }

            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const glm::mat4 matrix = transform.getMatrix();
            const glm::vec3 localMin(-radius,-height * 0.5f, - radius);
            const glm::vec3 localMax(radius,height * 0.5f, radius);
            glm::vec3 vertices[8];
            for (int i = 0; i < 8; i++) {
                glm::vec3 vertex(
                    (i & 1) ? localMax.x : localMin.x,
                    (i & 2) ? localMax.y : localMin.y,
                    (i & 4) ? localMax.z : localMin.z);
                vertices[i] = glm::vec3(matrix * glm::vec4(vertex, 1.0f));
            }
            glm::vec3 worldMin = vertices[0];
            glm::vec3 worldMax = vertices[0];
            for (auto vertice : vertices) {
                worldMin = glm::min(worldMin, vertice);
                worldMax = glm::max(worldMax, vertice);
            }
            const auto bB = BoundingBox(worldMin, worldMax);
            return bB;
        }


    private:
        float radius;
        float height;
};
