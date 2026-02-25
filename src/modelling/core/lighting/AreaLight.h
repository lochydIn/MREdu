//
// Created by Lochlan Harvey on 10/02/2026.
//

#pragma once
#include "Light.h"
#include "LightSample.h"

class AreaLight : public Light
{
public:
    virtual LightSample sample(const glm::vec3& hitPoint,
        float r1, float r2) const = 0;

    virtual float getArea() const = 0;
};
