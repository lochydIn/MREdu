//
// Created by Lochlan Harvey on 06/02/2026.
//
#pragma once
#include <cmath>
#include "../Entity.h"
#include "../components/Transform.h"
#include "../components/Material.h"

class Sphere : public Entity {
public:
    Sphere(const float radius, const glm::vec3& center, const std::shared_ptr<Material>& material)
        : Entity(material), center(center), radius(radius) {}

    Sphere(const Sphere&) = delete;
    Sphere& operator=(const Sphere&) = delete;

    bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
        const float a = dot(ray.direction , ray.direction);
        const float b = 2 * dot(ray.origin - center, ray.direction);
        const float c = dot(ray.origin - center, ray.origin -center) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
            // Calculate the intersection.
            float t = (-b - std::sqrt(discriminant)) / (2 * a);

            if (t > 0) { // If the intersection is within bounds.
                hit.point = ray.origin  + ray.direction * t; // Set the intersection point.
                const glm::vec3 pNormal = glm::normalize(hit.point - center);
                const float phi = atan2(pNormal.z, pNormal.x);
                const glm::vec3 outwardNormal = glm::normalize(hit.point - center);
                hit.setFrontSurface(ray, outwardNormal);
                hit.tangent = glm::vec3(-std::sin(phi), 0, std::cos(phi));
                hit.bitangent = glm::cross(hit.normal, hit.tangent);
                hit.uv.x = 0.5f + atan2(outwardNormal.z, outwardNormal.x) / (2 * M_PI);
                hit.uv.y = 0.5f - asin(outwardNormal.y) / M_PI;
                hit.distance = t;
                hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override {
        auto const bB = BoundingBox(center - glm::vec3(radius), center + glm::vec3(radius));
        return bB;
    }

    Transform transform;
    glm::vec3 center;
    float radius;
};
