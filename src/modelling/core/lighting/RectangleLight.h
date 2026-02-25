//
// Created by Lochlan Harvey on 19/02/2026.
//

#pragma once

#include "AreaLight.h"

class RectangleLight : public AreaLight
{
public:
    RectangleLight(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v,
                    const glm::vec3& colour, float intensity)
    : center(center),uAxis(u),vAxis(v),radiance(colour * intensity),colour(colour) {
    normal = glm::normalize(glm::cross(uAxis, vAxis));}

    LightSample sample(const glm::vec3& hitPoint,
        const float r1, const float r2) const override {
        LightSample sample;

        const glm::vec3 lightPoint = center + uAxis * (r1 - 0.5f) + vAxis * (r2 - 0.5f);
        const glm::vec3 toLight = lightPoint - hitPoint;
        const float area = 4.0f * glm::length(uAxis) * glm::length(vAxis);


        sample.xL = lightPoint;
        sample.lN = normal;
        sample.distance = glm::length(toLight);
        sample.w = toLight / sample.distance;
        sample.pdf = 1.0f / area;
        sample.Le = radiance;

        return sample;
    }

    float getArea() const override {
        4.0f * glm::length(uAxis) * glm::length(vAxis);
    }

    glm::vec3 getColour() const override {
        return colour;
    }


private:
    glm::vec3 center;
    glm::vec3 uAxis;
    glm::vec3 vAxis;
    glm::vec3 normal;
    glm::vec3 radiance;
    glm::vec3 colour;
};