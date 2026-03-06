//
// Created by Lochlan Harvey on 26/02/2026.
//

#pragma once
#include "../AreaLight.h"



class CuboidLight : public AreaLight {
    public:
        CuboidLight(const glm::vec3 center, glm::vec3 colour, float intensity, glm::vec3 min, glm::vec3 max)
        : AreaLight(colour, intensity), position(center), min(min), max(max) {

            glm::vec3 size = max - min;
            faceArea[0] = size.y * size.z;  //X
            faceArea[1] = size.x * size.z;  //Y
            faceArea[2] = size.x * size.y;  //Z
            totalArea = 2.0f * (faceArea[0] + faceArea[1] + faceArea[2]);
            invTotalArea = 1.0f / totalArea;

            auto mat = std::make_shared<Material>(colour,0.0f,0.0f,0.0f,1.0f,0.0f,glm::vec3(0.0f));
            radiance = (colour * intensity) / totalArea;
            mat->emissive = radiance * 10.0f;
            material = mat;
        }


        [[nodiscard]] LightSample sample(const glm::vec3& hitPoint, float r1, float r2) const override {
            LightSample sample{};

            const float faceChoice = r1 * 6.0f;
            const int face = static_cast<int>(faceChoice);
            const float faceR = faceChoice - face;

            const glm::vec3 worldMin = position + min;
            const glm::vec3 worldMax = position + max;

            switch (face) {
                case 0:
                    sample.xL = glm::vec3(
                        worldMax.x,
                        worldMin.y + faceR * (worldMax.y - worldMin.y),
                        worldMin.z + r2 * (worldMax.z - worldMin.z));
                    sample.lN = glm::vec3(1,0,0);
                    break;
                case 1:
                    sample.xL = glm::vec3(worldMax.x,
                    worldMin.y + faceR * (worldMax.y - worldMin.y),
                    worldMin.z + r2 * (worldMax.z - worldMin.z));
                    sample.lN = glm::vec3(-1,0,0);
                    break;
                case 2:
                    sample.xL = glm::vec3(
                        worldMin.x + faceR * (worldMax.x - worldMin.x),
                        worldMax.y,
                        worldMin.z + r2 * (worldMax.z - worldMin.z));
                    sample.lN = glm::vec3(0,1,0);
                    break;
                case 3:
                    sample.xL = glm::vec3(
                        worldMin.x + faceR * (worldMax.x - worldMin.x),
                        worldMax.y,
                        worldMin.z + r2 * (worldMax.z - worldMin.z));
                    sample.lN = glm::vec3(0,-1,0);
                    break;
                case 4:
                    sample.xL = glm::vec3(
                        worldMin.x + faceR * (worldMax.x - worldMin.x),
                        worldMin.y + r2 * (worldMax.y - worldMin.y),
                        worldMax.z);
                    sample.lN = glm::vec3(0,0,1);
                    break;
                case 5:
                    sample.xL = glm::vec3(
                            worldMin.x + faceR * (worldMax.x - worldMin.x),
                            worldMin.y + r2 * (worldMax.y - worldMin.y),
                            worldMin.z);
                    sample.lN = glm::vec3(0,0,-1);
                    break;
                default:
                sample.xL = position;
                sample.lN = glm::vec3(0,1,0);
                break;
            }
            sample.w = glm::normalize(sample.xL - hitPoint);
            sample.distance = glm::length(sample.xL - hitPoint);

            sample.Le = radiance;
            sample.pdf = invTotalArea;

            return sample;
        }


        bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
            glm::vec3 boxMin = position + min;
            glm::vec3 boxMax = position + max;

            float tEnter = -INFINITY;
            float tExit = INFINITY;

            for (int axis = 0; axis < 3; axis++) {
                const float origin = ray.origin[axis];
                const float direction = ray.direction[axis];

                const float minBound = boxMin[axis];
                const float maxBound = boxMax[axis];

                if (std::abs(direction) < 1e-8f) {
                    if (origin < minBound || origin > maxBound) {
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

                if (tEnter > tExit || tExit < tNear) {
                    return false;
                }
            }

            float t = tEnter;
            if (t < tMin) t = tExit;
            if (t > tMax || t < tMin) return false;

            hit.distance = t;
            hit.point = ray.positionAt(t);
            hit.entity = const_cast<CuboidLight*>(this);

            glm::vec3 localPoint = hit.point - position;
            glm::vec3 norm(0);

            float epsilon = 1e-6f;
            if (std::abs(localPoint.x - min.x) < epsilon) norm = glm::vec3(-1,0,0);
            else if (std::abs(localPoint.x - max.x) < epsilon) norm = glm::vec3(1,0,0);
            else if (std::abs(localPoint.y - min.y) < epsilon) norm = glm::vec3(0,-1,0);
            else if (std::abs(localPoint.y - max.y) < epsilon) norm = glm::vec3(0,1,0);
            else if (std::abs(localPoint.z - min.z) < epsilon) norm = glm::vec3(0,0,-1);
            else if (std::abs(localPoint.z - max.z) < epsilon) norm = glm::vec3(0,0,1);

            hit.normal = glm::normalize(norm);
            hit.setFrontSurface(ray, hit.normal);

            return true;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            auto const bB = BoundingBox(position + min, position + max);
            return bB;
        }

        [[nodiscard]] float getArea() const override {
            return totalArea;
        }

        [[nodiscard]] glm::vec3 getColour() const override {
            return colour;
        }




    private:
        glm::vec3 position;
        glm::vec3 min;
        glm::vec3 max;
        float faceArea[3];
        float totalArea;
        float invTotalArea;
};