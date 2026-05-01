//
// Created by Lochlan Harvey on 10/03/2026.
//
#pragma once
#include "../Entity.h"

class Triangle : public Entity
{
public:
    Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
             const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
             const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2,
             const std::shared_ptr<Material>& material) :
        Entity(material), v0(v0), v1(v1), v2(v2), n0(n0), n1(n1), n2(n2), uv0(uv0), uv1(uv1), uv2(uv2)
    {
        normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        computeTangents();
    }

    [[nodiscard]] std::string getName() const override
    {
        return "Triangle";
    }

    bool intersect(const Ray& ray, Intersection& hit, const float tMin, const float tMax) const override
    {
        constexpr float EPSILON = 0.0000001f;

        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;

        const glm::vec3 rayCrossEdgeTwo = glm::cross(ray.direction, edge2);

        const float determinant = glm::dot(edge1, rayCrossEdgeTwo);

        if (determinant > -EPSILON && determinant < EPSILON)
        {
            return false; // Ray parallel
        }

        const float invDeterminant = 1.0f / determinant;
        const glm::vec3 originToV0 = ray.origin - v0;
        const float u = invDeterminant * glm::dot(originToV0, rayCrossEdgeTwo);

        if (u < 0.0f || u > 1.0f)
        {
            return false;
        }

        const glm::vec3 originCrossEdgeOne = glm::cross(originToV0, edge1);
        const float v = invDeterminant * glm::dot(ray.direction, originCrossEdgeOne);

        if (v < 0.0f || u + v > 1.0f)
        {
            return false;
        }

        float t = invDeterminant * glm::dot(edge2, originCrossEdgeOne);

        if (t > tMin && t < tMax)
        {
            hit.distance = t;
            hit.point = ray.positionAt(t);
            const float w = 1.0f - u - v;
            const glm::vec3 interpolatedNormal = glm::normalize(w * n0 + u * n1 + v * n2);
            if (determinant < 0)
            {
                hit.normal = -interpolatedNormal;
            }
            else
            {
                hit.normal = interpolatedNormal;
            }
            hit.uv = w * uv0 + u * uv1 + v * uv2;
            hit.tangent = glm::normalize(w * tangent0 + u * tangent1 + v * tangent2);
            hit.bitangent = glm::normalize(w * bitangent0 + u * bitangent1 + v * bitangent2);

            hit.tangent = glm::normalize(hit.tangent - glm::dot(hit.tangent, hit.normal) * hit.normal);
            hit.bitangent = glm::cross(hit.normal, hit.tangent);
            hit.entity = const_cast<Triangle*>(this);
            hit.setFrontSurface(ray, hit.normal);
            return true;
        }
        return false;
    }

    [[nodiscard]] BoundingBox getBoundingBox() const override
    {
        const glm::vec3 min = glm::min(glm::min(v0, v1), v2);
        const glm::vec3 max = glm::max(glm::max(v0, v1), v2);
        const auto bB = BoundingBox(min, max);
        return bB;
    }

    void computeTangents()
    {
        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;

        const glm::vec2 dUV1 = uv1 - uv0;
        const glm::vec2 dUV2 = uv2 - uv0;
        const float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);
        glm::vec3 tangent;
        tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
        tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
        tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        glm::vec3 bitangent;
        bitangent.x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
        bitangent.y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
        bitangent.z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);

        tangent0 = tangent1 = tangent2 = tangent;
        bitangent0 = bitangent1 = bitangent2 = bitangent;
    }

private:
    glm::vec3 v0, v1, v2;
    glm::vec3 n0, n1, n2;
    glm::vec2 uv0, uv1, uv2;
    glm::vec3 tangent0, tangent1, tangent2;
    glm::vec3 bitangent0, bitangent1, bitangent2;
    glm::vec3 normal{};
};