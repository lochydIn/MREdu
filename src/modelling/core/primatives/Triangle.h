//
// Created by Lochlan Harvey on 10/03/2026.
//
#pragma once
#include "../Entity.h"

class Triangle : public Entity {
    public:
        Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const std::shared_ptr<Material>& material) :
            Entity(material),v0(v0),v1(v1),v2(v2) {
            normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        }

        bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override {
            constexpr float EPSILON = 0.00001f;

            const glm::vec3 edge1 = v1 - v0;
            const glm::vec3 edge2 = v2 - v0;

            const glm::vec3 rayCrossEdgeTwo = glm::cross(ray.direction, edge2);

            const float determinant = glm::dot(edge1, rayCrossEdgeTwo);

            if (determinant > -EPSILON && determinant < EPSILON) {
                return false;        // Ray parallel
            }

            const float invDeterminant = 1.0f / determinant;
            const glm::vec3 originToV0 = ray.origin - v0;
            const float u = invDeterminant * glm::dot(originToV0, rayCrossEdgeTwo);

            if (u < 0.0f || u > 1.0f) {
                return false;
            }

            const glm::vec3 originCrossEdgeOne = glm::cross(originToV0, edge1);
            float v = invDeterminant * glm::dot(ray.direction, originCrossEdgeOne);

            if (v < 0.0f || u + v > 1.0f) {
                return false;
            }

            float t = invDeterminant * glm::dot(edge2, originCrossEdgeOne);

            if (t > tMin && t < tMax) {
                hit.distance = t;
                hit.point = ray.positionAt(t);
                if (determinant < 0) {
                    hit.normal = -normal;
                } else {
                    hit.normal = normal;
                }

                hit.entity = const_cast<Triangle*>(this);
                hit.setFrontSurface(ray,hit.normal);
                return true;
            }
            return false;
        }

        [[nodiscard]] BoundingBox getBoundingBox() const override {
            const glm::vec3 min = glm::min(glm::min(v0,v1),v2);
            const glm::vec3 max = glm::max(glm::max(v0,v1),v2);
            const auto bB = BoundingBox(min, max);
            return bB;
        }

    private:
        glm::vec3 v0, v1, v2;
        glm::vec3 normal{};
};
