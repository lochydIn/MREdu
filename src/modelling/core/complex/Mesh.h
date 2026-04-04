//
// Created by Lochlan Harvey on 10/03/2026.
//
#pragma once
#include "../primatives/Triangle.h"
#include "../BVH.h"

class Mesh : public Entity {
    public:
        Mesh(const glm::vec3& position, const std::vector<Triangle*>& triangles,
            const std::shared_ptr<Material>& material)
            : Entity(material), position(position),triangles(triangles) {

            std::vector<Entity*> triangleEntities;
            for (const auto triangle : triangles) {
                triangleEntities.push_back(triangle);
            }

            meshBVH = std::make_unique<BVH>(triangleEntities);

            for (const auto& triangle : triangles) {
                local.expand(triangle->getBoundingBox());
            }

            world = BoundingBox(local.min + position,local.max + position);
        }

        ~Mesh() override{
            for (const auto triangle : triangles) {
                delete triangle;
            }
        }

        bool intersect(const Ray& worldRay, Intersection& hit, const float tMin, const float tMax) const override {
            const Ray ray = worldToObject(worldRay);

            if (meshBVH->intersect(ray,hit)) {
                objectToWorld(hit);
                hit.point = hit.point + position;
                return true;
            }
            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            return world;
        }


    private:
        glm::vec3 position;
        std::vector<Triangle*> triangles;
        std::unique_ptr<BVH> meshBVH;
        BoundingBox local;
        BoundingBox world;
};