//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include "../../rendering/structs/Intersection.h"
#include "Entity.h"
#include "../../rendering/BVH.h"
#include "lighting/Light.h"
#include "primatives/Plane.h"


class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void addEntity(Entity* entity)
    {
        if (dynamic_cast<Plane*>(entity))
        {
            infiniteEntities.push_back(entity);
        }
        else
        {
            finiteEntities.push_back(entity);
        }
    };

    void removeEntity(const Entity* entity)
    {
        auto ent = std::ranges::find(finiteEntities, entity);
        if (ent != finiteEntities.end())
        {
            finiteEntities.erase(ent);
            delete entity;
            return;
        }
        auto plane = std::ranges::find(infiniteEntities, entity);
        if (ent != infiniteEntities.end())
        {
            infiniteEntities.erase(plane);
            delete entity;
        }
        resetBVH();
    };


    void addLight(Light* light)
    {
        lights.push_back(light);
    };

    void removeLight(Light* light)
    {
        auto li = std::ranges::find(lights, light);
        if (li != lights.end())
        {
            lights.erase(li);
        }
        if (const auto entity = dynamic_cast<Entity*>(light))
        {
            auto ent = std::ranges::find(finiteEntities, entity);
            if (ent != finiteEntities.end())
            {
                finiteEntities.erase(ent);
            }
        }
        delete light;
        resetBVH();
    };

    [[nodiscard]] std::vector<Light*> getLights() const
    {
        return lights;
    };

    [[nodiscard]] std::vector<Entity*> getFiniteEntities() const
    {
        return finiteEntities;
    }

    [[nodiscard]] std::vector<Entity*> getInfiniteEntities() const
    {
        return infiniteEntities;
    }

    [[nodiscard]] Intersection intersect(const Ray& ray) const
    {
        Intersection hit;
        hit.distance = -1;
        float closest = 1e30f;
        constexpr float tMin = 0.001f;

        if (bvh)
        {
            Intersection bvhHit;
            if (bvh->intersect(ray, bvhHit) && bvhHit.distance < closest)
            {
                closest = bvhHit.distance;
                hit = bvhHit;
            }
        }
        for (auto plane : infiniteEntities)
        {
            Intersection planeHit;
            if (plane->intersect(ray, planeHit, tMin, closest) &&
                planeHit.distance < closest && planeHit.distance > 0)
            {
                closest = planeHit.distance;
                hit = planeHit;
            }
        }
        return hit;
    }

    void buildBVH()
    {
        if (!finiteEntities.empty())
        {
            bvh = std::make_unique<BVH>(finiteEntities);
        }
        else
        {
            bvh.reset();
        }
    }

    void resetBVH()
    {
        bvh.reset();
    }

private:
    std::vector<Entity*> finiteEntities;
    std::vector<Entity*> infiniteEntities;
    std::vector<Light*> lights;
    std::unique_ptr<BVH> bvh;
};