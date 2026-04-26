//
// Created by Lochlan Harvey on 10/03/2026.
//
#pragma once
#include "../primatives/Triangle.h"
#include "../../../rendering/BVH.h"

class Mesh : public Entity
{
public:
    Mesh(const std::vector<Triangle*>& triangles, const std::shared_ptr<Material>& material)
        : Entity(material), triangles(triangles)
    {
        std::vector<Entity*> triangleEntities;
        for (const auto triangle : triangles)
        {
            triangleEntities.push_back(triangle);
        }

        meshBVH = std::make_unique<BVH>(triangleEntities);

        for (const auto& triangle : triangles)
        {
            local.expand(triangle->getBoundingBox());
        }
    }

    [[nodiscard]] std::string getName() const override
    {
        return "MeshObject";
    }

    ~Mesh() override
    {
        for (const auto triangle : triangles)
        {
            delete triangle;
        }
    }

    bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override
    {
        const Ray localRay = rayToObjectSpace(ray);
        if (meshBVH->intersect(localRay, hit))
        {
            objectIntersectionToWorldSpace(hit);
            return true;
        }
        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override
    {
        const glm::mat4 matrix = transform.getMatrix();
        glm::vec3 vertices[8];
        for (int i = 0; i < 8; i++)
        {
            glm::vec3 vertex(
                (i & 1) ? local.max.x : local.min.x,
                (i & 2) ? local.max.y : local.min.y,
                (i & 4) ? local.max.z : local.min.z);
            vertices[i] = glm::vec3(matrix * glm::vec4(vertex, 1.0f));
        }
        glm::vec3 worldMin = vertices[0];
        glm::vec3 worldMax = vertices[0];
        for (int i = 1; i < 8; i++)
        {
            worldMin = glm::min(worldMin, vertices[i]);
            worldMax = glm::max(worldMax, vertices[i]);
        }
        const auto bB = BoundingBox(worldMin, worldMax);
        return bB;
    }

private:
    std::vector<Triangle*> triangles;
    std::unique_ptr<BVH> meshBVH;
    BoundingBox local;
};