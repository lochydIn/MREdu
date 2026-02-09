//
// Created by Lochlan Harvey on 06/02/2026.
//
#pragma once
#include <cmath>
#include <limits>
#include "../../../rendering/Intersection.h"
#include "../../../rendering/Ray.h"
#include "../Entity.h"
#include "../../components/Transform.h"
#include "../../components/Material.h"

class Sphere : public Entity {
public:
    Sphere(const float radius, const glm::vec3& center, const std::shared_ptr<Material>& material)
        : Entity(material), center(center), radius(radius) {}

    Sphere(const Sphere&) = delete;
    Sphere& operator=(const Sphere&) = delete;

    bool intersect(const Ray& ray, Intersection& hit) const override {
        float a = dot(ray.direction , ray.direction);
        float b = 2 * dot(ray.origin - center, ray.direction);
        float c = dot(ray.origin - center, ray.origin -center) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
            const float t = (-b - std::sqrt(discriminant)) / (2 * a); // Calculate the intersection.
            if (t > 0) { // If the intersection is in front of the camera.
                hit.point = ray.origin  + ray.direction * t; // Set the intersection point.
                glm::vec3 outwardNormal = glm::normalize(hit.point - center); // Set the outward normal sphere center to intersection.
                hit.setFrontSurface(ray, outwardNormal); // Set the front surface (will be true until I introduce refraction)
                hit.distance = t;
                hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                return true;
            }
        }
        return false;
    }

    Transform transform;
    glm::vec3 center;
    float radius;
};
