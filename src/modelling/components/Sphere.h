//
// Created by Lochlan Harvey on 02/02/2026.
//

# pragma once
#include <cmath>
#include <string>

# include "Component.h"
# include "../../rendering/Intersection.h"
#include "../../rendering/Ray.h"


class Sphere : public Component {
public:
    Sphere(const std::string& name, const float radius, const glm::vec3 center)
        : Component(name), center(center), radius(radius) {}

    bool intersect(const Ray& ray, Intersection& intersection) const
    {
        float a = dot(ray.direction , ray.direction);
        float b = 2 * dot(ray.origin - center, ray.direction);
        float c = dot(ray.origin - center, ray.origin -center) - radius * radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false;
        } else {
            const float first = (-b - std::sqrt(discriminant)) / (2 * a);
            return (first > 0);
        }
    }


private:
    glm::vec3 center;
    float radius;

};

