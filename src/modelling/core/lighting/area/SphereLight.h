//
// Created by Lochlan Harvey on 26/02/2026.
//

#pragma once
#include "../AreaLight.h"
#include "glm/ext/scalar_constants.hpp"

class SphereLight : public AreaLight {
    public:
        SphereLight(glm::vec3 center, const float radius, glm::vec3 colour, const float intensity)
            : AreaLight(colour, intensity), position(center), radius(radius) {
            const auto mat = std::make_shared<Material>(colour,0.0f,0.0f,0.0f,1.0f,
                0.0f,glm::vec3(0.0f));
            area = 4.0f * glm::pi<float>() * radius * radius;
            invArea = 1.0f / area;
            radiance = colour * intensity / area;
            mat->emissive = radiance * 10.0f;
            material = mat;

        }

        [[nodiscard]] LightSample sample(const glm::vec3& hitPoint, const float r1, const float r2) const override {
            LightSample sample{};

            const float theta = 2.0f * glm::pi<float>() * r1;
            const float phi = glm::acos(2.0f * r2 - 1.0f);

            glm::vec3 direction(
                glm::sin(phi) * glm::cos(theta),
                glm::sin(phi) * glm::sin(theta),
                glm::cos(phi));


            sample.xL = position + direction * radius;
            sample.lN = glm::normalize(sample.xL - position);
            sample.w = glm::normalize(sample.xL - hitPoint);
            sample.distance = glm::length(sample.xL - hitPoint);
            sample.Le = radiance;
            sample.pdf = invArea;
            return sample;
        }

        bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override {
            const float a = dot(ray.direction , ray.direction);
            const float b = 2 * dot(ray.origin - position, ray.direction);
            const float c = dot(ray.origin - position, ray.origin - position) - radius * radius;
            float discriminant = b * b - 4 * a * c;

            if (discriminant >= std::numeric_limits<float>::epsilon()) {   // If the ray intersects with the sphere.
                // Calculate the intersection.
                float t = (-b - std::sqrt(discriminant)) / (2 * a);

                if (t > 0) { // If the intersection is within bounds.
                    hit.point = ray.origin  + ray.direction * t; // Set the intersection point.
                    const glm::vec3 outwardNormal = glm::normalize(hit.point - position);
                    hit.setFrontSurface(ray, outwardNormal);
                    hit.distance = t;
                    hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            auto const bB = BoundingBox(position - glm::vec3(radius), position + glm::vec3(radius));
            return bB;
        }

        [[nodiscard]] float getArea() const override{
            return area;
        }

        [[nodiscard]] glm::vec3 getColour() const override {
            return colour;
        }


    private:
        glm::vec3 position;
        float radius;
        float area;
        float invArea;
};
