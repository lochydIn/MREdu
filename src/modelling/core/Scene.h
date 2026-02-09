//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include "lighting/DirectionalLight.h"
#include "../../rendering/Intersection.h"
#include "Entity.h"
#include "lighting/Light.h"


class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void addEntity(Entity* entity) {
        entities.push_back(entity);
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
        Intersection closestHit;
        closestHit.distance = FLT_MAX;

        for (const Entity* entity : entities) {
            Intersection hit;
            hit.distance = FLT_MAX;

            if (entity->intersect(ray,hit)) {
                if(hit.distance < closestHit.distance && (hit.distance > 0)) {
                    closestHit = hit;
                }
            }
        }

        if (closestHit.distance == FLT_MAX) {
            closestHit.distance = -1.0f;
        }

        return closestHit;
    }

private:
    std::vector<Entity*> entities;
    std::vector<Light*> lights;
};
