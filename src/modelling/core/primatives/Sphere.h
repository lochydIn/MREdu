//
// Created by Lochlan Harvey on 06/02/2026.
//
#pragma once
#include <cmath>
#include "../Entity.h"
#include "../components/Transform.h"
#include "../components/Material.h"
#include <iostream>

class Sphere : public Entity {
    public:
        Sphere(const std::shared_ptr<Material>& material)
            : Entity(material) {}

        Sphere(const Sphere&) = delete;
        Sphere& operator=(const Sphere&) = delete;

        [[nodiscard]] std::string getName() const override {
            return "Sphere";
        }

        bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
            Ray localRay = rayToObjectSpace(ray);
            const float a = dot(localRay.direction , localRay.direction);
            const float b = 2 * dot(localRay.origin, localRay.direction);
            const float c = dot(localRay.origin, localRay.origin) - 1.0f;
            float discriminant = b * b - 4 * a * c;

            if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
                // Calculate the intersection.
                float t = (-b - std::sqrt(discriminant)) / (2 * a);

                if (t > 0) { // If the intersection is within bounds.
                    hit.distance = t;
                    hit.point = localRay.positionAt(t);
                    hit.normal = glm::normalize(hit.point);

                    const float phi = atan2(hit.point.z, hit.point.x);;
                    hit.uv.x = 0.5f + phi / (2 * M_PI);
                    hit.uv.y = 0.5f - asin(hit.point.y) / M_PI;

                    hit.tangent = glm::vec3(-std::sin(phi), 0, std::cos(phi));
                    hit.bitangent = glm::cross(hit.normal, hit.tangent);

                    objectIntersectionToWorldSpace(hit);
                    hit.setFrontSurface(ray,hit.normal);
                    hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                    return true;
                }
            }
            return false;
        }



        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const float worldRadius = glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z));
            const auto bB =  BoundingBox(transform.position - worldRadius, transform.position + worldRadius);
            return bB;
        }
};
