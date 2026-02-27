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
        mat->emissive = radiance;
        material = mat;

    }

    [[nodiscard]] LightSample sample(const glm::vec3& hitPoint,const float r1, const float r2) const override {

        LightSample sample{};
        const glm::vec3 lightPoint = center + uAxis * (r1 - 0.5f) + vAxis * (r2 - 0.5f);
        const glm::vec3 toLight = lightPoint - hitPoint;
        const float area = 4.0f * glm::length(uAxis) * glm::length(vAxis);


        sample.xL = lightPoint;
        sample.distance = glm::length(toLight);
        sample.w = toLight / sample.distance;
        sample.pdf = 1.0f / area;
        sample.Le = radiance;

        return sample;
    }

    // From Entity
    bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override;

    [[nodiscard]] BoundingBox getBoundingBox() const override {}




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
};