//
// Created by Lochlan Harvey on 02/02/2026.
//

# pragma once
#include <cmath>
#include <string>

# include "Component.h"
# include "../../rendering/Intersection.h"
#include "../../rendering/Ray.h"
#include <limits>

class Sphere : public Component {
public:
    Sphere(const float radius, const glm::vec3 center)
        : center(center), radius(radius) {}

    bool intersect(const Ray& ray, Intersection& intersection) const
    {
        float a = dot(ray.direction , ray.direction);
        float b = 2 * dot(ray.origin - center, ray.direction);
        float c = dot(ray.origin - center, ray.origin -center) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
            const float t = (-b - std::sqrt(discriminant)) / (2 * a); // Calculate the intersection.
            if (t > 0) { // If the intersection is in front of the camera.
                intersection.point = ray.origin  + ray.direction * t; // Set the intersection point.
                glm::vec3 outwardNormal = glm::normalize(intersection.point - center); // Set the outward normal sphere center to intersection.
                intersection.setFrontSurface(ray, outwardNormal); // Set the front surface (will be true until I introduce refraction)
                intersection.distance = t;
                return true;
            }
        }
        return false;
    }


private:
    glm::vec3 center;
    float radius;

};

