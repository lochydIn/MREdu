//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include "../../rendering/structs/Intersection.h"
#include "Entity.h"
#include "BVH.h"
#include "lighting/Light.h"
#include "primatives/Plane.h"


class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void addEntity(Entity* entity) {
        if (dynamic_cast<Plane*>(entity)) {
            infiniteEntities.push_back(entity);
        } else {
            finiteEntities.push_back(entity);
        }

    };

    static void removeEntity(){};

    void addLight(Light* light) {
        lights.push_back(light);
    };

    void static removeLight(){};

    [[nodiscard]] std::vector<Light*> getLights() const {
        return lights;
    };

    [[nodiscard]] Intersection intersect(const Ray& ray) const {
        Intersection hit;
        hit.distance = -1;
        float closest = 1e30f;
        constexpr float tMin = 0.001f;

        if (bvh) {
            Intersection bvhHit;
            if (bvh->intersect(ray, bvhHit) && bvhHit.distance < closest)
            {
                closest = bvhHit.distance;
                hit = bvhHit;
            }
        }

        for (auto plane : infiniteEntities) {
            Intersection planeHit;
            if (plane->intersect(ray, planeHit,tMin,closest) &&
                planeHit.distance < closest && planeHit.distance > 0) {
                closest = planeHit.distance;
                hit = planeHit;
            }
        }
        return hit;
    }

    void buildBVH() {
        if (!finiteEntities.empty()) {
            bvh = std::make_unique<BVH>(finiteEntities);
        }
    }



private:
    std::vector<Entity*> finiteEntities;
    std::vector<Entity*> infiniteEntities;
    std::vector<Light*> lights;
    std::unique_ptr<BVH> bvh;
};
