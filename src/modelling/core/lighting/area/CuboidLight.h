//
// Created by Lochlan Harvey on 26/02/2026.
//

#pragma once
#include "../AreaLight.h"

class CuboidLight : public AreaLight
{
public:
    CuboidLight()
        : AreaLight(glm::vec3(1.0f), 10.0f)
    {
        max = glm::vec3(0.5f, 0.5f, 0.5f);
        min = glm::vec3(-0.5f, -0.5f, -0.5f);
        glm::vec3 size = max - min;
        faceArea[0] = size.y * size.z; //X
        faceArea[1] = size.x * size.z; //Y
        faceArea[2] = size.x * size.y; //Z
        area = 2.0f * (faceArea[0] + faceArea[1] + faceArea[2]);
        invTotalArea = 1.0f / area;

        auto mat = std::make_shared<Material>(colour, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, glm::vec3(0.0f));
        radiance = (colour * intensity) / area;
        mat->emissive = radiance * 10.0f;
        material = mat;
    }


    [[nodiscard]] LightSample sample(const glm::vec3& hitPoint, float r1, float r2) const override
    {
        LightSample sample{};

        const float faceChoice = r1 * 6.0f;
        const int face = static_cast<int>(faceChoice);
        const float faceR = faceChoice - face;

        const glm::vec3 worldMin = transform.position + min;
        const glm::vec3 worldMax = transform.position + max;

        switch (face)
        {
        case 0:
            sample.xL = glm::vec3(
                worldMax.x,
                worldMin.y + faceR * (worldMax.y - worldMin.y),
                worldMin.z + r2 * (worldMax.z - worldMin.z));
            sample.lN = glm::vec3(1, 0, 0);
            break;
        case 1:
            sample.xL = glm::vec3(worldMax.x,
                                  worldMin.y + faceR * (worldMax.y - worldMin.y),
                                  worldMin.z + r2 * (worldMax.z - worldMin.z));
            sample.lN = glm::vec3(-1, 0, 0);
            break;
        case 2:
            sample.xL = glm::vec3(
                worldMin.x + faceR * (worldMax.x - worldMin.x),
                worldMax.y,
                worldMin.z + r2 * (worldMax.z - worldMin.z));
            sample.lN = glm::vec3(0, 1, 0);
            break;
        case 3:
            sample.xL = glm::vec3(
                worldMin.x + faceR * (worldMax.x - worldMin.x),
                worldMax.y,
                worldMin.z + r2 * (worldMax.z - worldMin.z));
            sample.lN = glm::vec3(0, -1, 0);
            break;
        case 4:
            sample.xL = glm::vec3(
                worldMin.x + faceR * (worldMax.x - worldMin.x),
                worldMin.y + r2 * (worldMax.y - worldMin.y),
                worldMax.z);
            sample.lN = glm::vec3(0, 0, 1);
            break;
        case 5:
            sample.xL = glm::vec3(
                worldMin.x + faceR * (worldMax.x - worldMin.x),
                worldMin.y + r2 * (worldMax.y - worldMin.y),
                worldMin.z);
            sample.lN = glm::vec3(0, 0, -1);
            break;
        default:
            sample.xL = transform.position;
            sample.lN = glm::vec3(0, 1, 0);
            break;
        }
        sample.w = glm::normalize(sample.xL - hitPoint);
        sample.distance = glm::length(sample.xL - hitPoint);

        sample.Le = radiance;
        sample.pdf = invTotalArea;

        return sample;
    }


    bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const override
    {
        Ray localRay = rayToObjectSpace(ray);

        float tEnter = -INFINITY;
        float tExit = INFINITY;

        for (int axis = 0; axis < 3; axis++)
        {
            const float origin = localRay.origin[axis];
            const float direction = localRay.direction[axis];

            const float minBound = min[axis];
            const float maxBound = max[axis];

            if (std::abs(direction) < 1e-8f)
            {
                if (origin < minBound || origin > maxBound)
                {
                    return false;
                }
                continue;
            }
            float t1 = (minBound - origin) / direction;
            float t2 = (maxBound - origin) / direction;
            float tNear = std::min(t1, t2);
            float tFar = std::max(t1, t2);

            tEnter = std::max(tEnter, tNear);
            tExit = std::min(tExit, tFar);

            if (tEnter > tExit || tExit < tNear)
            {
                return false;
            }
        }

        float t = tEnter;
        if (t < tMin) t = tExit;
        if (t > tMax || t < tMin) return false;

        hit.distance = t;
        hit.point = localRay.positionAt(t);

        glm::vec3 localNormal;
        glm::vec3 localPoint = hit.point;
        glm::vec2 uv;
        glm::vec3 size = max - min;

        float epsilon = 1e-6f;

        if (std::abs(localPoint.x - min.x) < epsilon)
        {
            localNormal = glm::vec3(-1, 0, 0);
            uv = glm::vec2(
                (localPoint.z - min.z) / size.z,
                (localPoint.y - min.y) / size.y);
            hit.tangent = glm::vec3(0, 0, 1);
            hit.bitangent = glm::vec3(0, 1, 0);
        }
        else if (std::abs(localPoint.x - max.x) < epsilon)
        {
            localNormal = glm::vec3(1, 0, 0);
            uv = glm::vec2(
                1.0f - (localPoint.z - min.z) / size.z,
                (localPoint.y - min.y) / size.y);
            hit.tangent = glm::vec3(0, 0, 1);
            hit.bitangent = glm::vec3(0, 1, 0);
        }
        else if (std::abs(localPoint.y - min.y) < epsilon)
        {
            localNormal = glm::vec3(0, -1, 0);
            uv = glm::vec2(
                (localPoint.x - min.x) / size.x,
                (localPoint.z - min.z) / size.z);
            hit.tangent = glm::vec3(1, 0, 0);
            hit.bitangent = glm::vec3(0, 0, 1);
        }
        else if (std::abs(localPoint.y - max.y) < epsilon)
        {
            localNormal = glm::vec3(0, 1, 0);
            uv = glm::vec2(
                1.0f - (localPoint.x - min.x) / size.x,
                (localPoint.z - min.z) / size.z);
            hit.tangent = glm::vec3(1, 0, 0);
            hit.bitangent = glm::vec3(0, 0, 1);
        }
        else if (std::abs(localPoint.z - min.z) < epsilon)
        {
            localNormal = glm::vec3(0, 0, -1);
            uv = glm::vec2(
                (localPoint.x - min.x) / size.x,
                (localPoint.y - min.y) / size.y);
            hit.tangent = glm::vec3(1, 0, 0);
            hit.bitangent = glm::vec3(0, 1, 0);
        }
        else if (std::abs(localPoint.z - max.z) < epsilon)
        {
            localNormal = glm::vec3(0, 0, 1);
            uv = glm::vec2(
                1.0f - (localPoint.x - min.x) / size.x,
                (localPoint.y - min.y) / size.y);
            hit.tangent = glm::vec3(1, 0, 0);
            hit.bitangent = glm::vec3(0, 1, 0);
        }
        hit.normal = localNormal;
        hit.uv = uv;

        hit.tangent = glm::normalize(hit.tangent - glm::dot(hit.tangent, hit.normal) * hit.normal);
        hit.bitangent = glm::cross(hit.normal, hit.tangent);

        objectIntersectionToWorldSpace(hit);
        hit.entity = const_cast<Entity*>(dynamic_cast<const Entity*>(this));
        hit.setFrontSurface(ray, hit.normal);
        return true;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override
    {
        const glm::mat4 matrix = transform.getMatrix();

        glm::vec3 vertices[8];
        for (int i = 0; i < 8; i++)
        {
            glm::vec3 localVertex(
                (i & 1) ? max.x : min.x, (i & 2) ? max.y : min.y, (i & 4) ? max.z : min.z);
            vertices[i] = glm::vec3(matrix * glm::vec4(localVertex, 1.0f));
        }
        glm::vec3 worldMin = vertices[0];
        glm::vec3 worldMax = vertices[0];
        for (int i = 1; i < 8; i++)
        {
            worldMin = glm::min(worldMin, vertices[i]);
            worldMax = glm::max(worldMax, vertices[i]);
        }

        auto const bB = BoundingBox(worldMin, worldMax);
        return bB;
    }

private:
    glm::vec3 min;
    glm::vec3 max;
    float faceArea[3];
    float invTotalArea;
};