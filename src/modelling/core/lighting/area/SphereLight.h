//
// Created by Lochlan Harvey on 26/02/2026.
//

#pragma once
#include "../AreaLight.h"
#include "glm/ext/scalar_constants.hpp"

class SphereLight : public AreaLight {
    public:
        SphereLight()
            : AreaLight(glm::vec3(1.0f), 10.0f) {
            transform.position = glm::vec3(0,0,0);
            radius = 1.0f;
            area = 4.0f * glm::pi<float>() * radius * radius;
            invArea = 1.0f / area;
            radiance = colour * intensity / area;

            const auto mat = std::make_shared<Material>(colour,0.0f,0.0f,0.0f,1.0f,
               0.0f,glm::vec3(0.0f));
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


            sample.xL = transform.position + direction * radius;
            sample.lN = glm::normalize(sample.xL - transform.position);
            sample.w = glm::normalize(sample.xL - hitPoint);
            sample.distance = glm::length(sample.xL - hitPoint);
            sample.Le = radiance;
            sample.pdf = invArea;
            return sample;
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
    private:
        float radius;
        float invArea;
};
