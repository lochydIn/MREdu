//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include <memory>
#include "BoundingBox.h"
#include "components/Material.h"

struct Intersection;

class Entity {
public:
// Constructor
        Entity() = default;
        Entity(std::shared_ptr<Material> material) : material(std::move(material)) {}

// Deconstructor
        virtual ~Entity() = default;

//Key Methods
        virtual bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const = 0;

        const Material& getMaterial() const { return *material; }
        static void setMaterial(std::unique_ptr<Material>material) {material = std::move(material);}

        virtual BoundingBox getBoundingBox() const = 0;

// Copy Prevention
        Entity(const Entity&) = delete;
        Entity& operator=(const Entity&) = delete;



protected:
std::shared_ptr<const Material> material = std::make_shared<const Material>();
};


