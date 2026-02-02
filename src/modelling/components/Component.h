//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include "../../rendering/Intersection.h"
#include "../../rendering/Ray.h"

class Component {
    public:
    // Constructor
    Component(const std::string& name) {
                m_name = name;
            }

    virtual ~Component() = default;

    virtual void update() {}

    virtual bool intersect(Ray& ray, Intersection& intersection)
    {
        return false;
    }

    private:
        std::string m_name;

};



