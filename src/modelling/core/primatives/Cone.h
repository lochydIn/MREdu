//
// Created by Lochlan Harvey on 25/02/2026.
//

#pragma once
#include <memory>
#include "../Entity.h"
#include "../components/Material.h"
#include "../components/Transform.h"

class Cone : public Entity {
    public:
        Cone(const glm::vec3 position, const float height, const float radius, const std::shared_ptr<Material>& mat)
            : Entity(mat), position(glm::vec3(position.x,position.y + height, position.z)), radius(radius), height(height * 2) {}

        Cone(const Cone&) = delete;

        Cone& operator=(const Cone&) = delete;

        bool intersect (const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
            glm::vec3 ro = ray.origin - position;
            glm::vec3 rd = ray.direction;

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
                if (!(ray.positionAt(t1).y > position.y)) {
                    // Check both intersections
                    if (t1 > tMin) {
                        float y = ro.y + t1 * rd.y;
                        if (y >= -height * 0.5f && y <= height * 0.5f) {
                            t = t1;
                            glm::vec3 point = ro + rd * t1;
                            glm::vec3 normalDir = glm::vec3(point.x, -k * k * point.y, point.z);
                            normal = glm::normalize(normalDir);
                        }
                    }
                }
                if (!(ray.positionAt(t2).y > position.y)) {
                    if (t2 > tMin && t2 < t) {
                        float y = ro.y + t2 * rd.y;
                        if (y >= -height * 0.5f && y <= height * 0.5f) {
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
                hit.entity = const_cast<Cone*>(this);
                hit.setFrontSurface(ray, hit.normal);
                return true;
            }

            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const float maxR = radius;
            const auto bB = BoundingBox(position - glm::vec3(maxR,height * 0.5f,maxR),
                position + glm::vec3(maxR,height * 0.5f,maxR));
            return bB;
        }


    private:
        glm::vec3 position;
        float radius;
        float height;
        Transform transform;
};
