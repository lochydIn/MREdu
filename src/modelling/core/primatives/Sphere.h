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

    bool intersect(const Ray& ray, Intersection& intersection, const float tMin, const float tMax) const override {
        const float a = dot(ray.direction , ray.direction);
        const float b = 2 * dot(ray.origin - center, ray.direction);
        const float c = dot(ray.origin - center, ray.origin -center) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
            // Calculate the intersection.
            float t = (-b - std::sqrt(discriminant)) / (2 * a);

            if (t > 0) { // If the intersection is within bounds.
                intersection.point = ray.origin  + ray.direction * t; // Set the intersection point.
                const glm::vec3 outwardNormal = glm::normalize(intersection.point - center);
                intersection.setFrontSurface(ray, outwardNormal);
                intersection.distance = t;
                intersection.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
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
