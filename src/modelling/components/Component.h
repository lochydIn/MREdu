//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include "../../rendering/Ray.h"

struct Intersection;

class Component {
    public:
    // Constructor
    Component() = default;

    Component(const Component&) = default;
    Component& operator=(const Component&) = default;

    virtual ~Component() = default;

    virtual void update() {}

    virtual bool intersect(Ray& ray, Intersection& intersection)
    {
        return false;
    }
};



