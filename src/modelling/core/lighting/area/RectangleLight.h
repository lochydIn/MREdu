//
// Created by Lochlan Harvey on 19/02/2026.
//

#pragma once

#include "../AreaLight.h"

class RectangleLight : public AreaLight {
public:
    RectangleLight(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v,const glm::vec3& colour, float intensity)
        : AreaLight(colour, intensity), center(center), uAxis(u), vAxis(v) {

        auto mat = std::make_shared<Material>(colour,0.0f,0.0f,0.0f,1.0f,
            0.0f,glm::vec3(0.0f));
        area = 4.0f * glm::length(uAxis) * glm::length(vAxis);
        radiance = (colour * intensity) / area;
        mat->emissive = radiance * 10.0f;
        material = mat;
    }

    [[nodiscard]] LightSample sample(const glm::vec3& hitPoint,const float r1, const float r2) const override {

        LightSample sample{};
        const glm::vec3 lightPoint = center + uAxis * (r1 - 0.5f) + vAxis * (r2 - 0.5f);
        const glm::vec3 toLight = lightPoint - hitPoint;



        sample.xL = lightPoint;
        sample.lN = glm::normalize(glm::cross(uAxis, vAxis));
        sample.distance = glm::length(toLight);
        sample.w = toLight / sample.distance;
        sample.pdf = 1.0f / area;
        sample.Le = radiance;

        return sample;
    }

    // From Entity
    bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
        glm::vec3 normal = glm::normalize(glm::cross(uAxis, vAxis));

        const float denom = glm::dot(normal, ray.direction);
        if (glm::abs(denom) < 1e-6f) return false;

        const float t = glm::dot(center - ray.origin, normal) / denom;
        if (t < tMin || t > tMax) return false;

        const glm::vec3 point = ray.positionAt(t);
        const glm::vec3 localPoint = point - center;

        const float u = glm::dot(localPoint, glm::normalize(uAxis));
        const float v = glm::dot(localPoint, glm::normalize(vAxis));

        if (glm::abs(u) <= glm::length(uAxis) * 0.5f && glm::abs(v) <= glm::length(vAxis) * 0.5f) {
            hit.distance = t;
            hit.point = point;
            hit.normal = (denom < 0) ? normal : -normal;
            hit.entity = const_cast<RectangleLight*>(this);
            hit.setFrontSurface(ray, hit.normal);
            return true;
        }

        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override {
        const glm::vec3 halfU = uAxis * 0.5f;
        const glm::vec3 halfV = vAxis * 0.5f;

        const glm::vec3 corners[4] = {
            center - halfU - halfV,
            center + halfU - halfV,
            center - halfU + halfV,
            center + halfU + halfV };

        glm::vec3 min = corners[0];
        glm::vec3 max = corners[0];

        for (int i = 1; i < 4; i++) {
            min = glm::min(min, corners[i]);
            max = glm::max(max, corners[i]);
        }
        min -= glm::vec3(0.01f);
        max += glm::vec3(0.01f);

        const auto bB = BoundingBox(min, max);
        return bB;

    }




    [[nodiscard]] float getArea() const override {
        return 4.0f * glm::length(uAxis) * glm::length(vAxis);
    }

    [[nodiscard]] glm::vec3 getColour() const override {
        return colour;
    }


private:
    glm::vec3 center;
    glm::vec3 uAxis;
    glm::vec3 vAxis;
    float area;
};