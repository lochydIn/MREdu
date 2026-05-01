//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "Texture.h"
#include <memory>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

class Material
{
public:
    // General Properties
    glm::vec3 colour;
    float roughness;
    float metallic;
    float reflectivity;
    float iOR;
    float transparency;
    glm::vec3 attenuation;
    glm::vec3 emissive;

    // Extended Properties
    float clearcoat;
    float clearcoatRoughness;
    float anisotropy;
    float anisotropyRotation;
    float sheen;
    glm::vec3 sheenColour;

    // Texture Maps and Controls
    float colourMapIntensity;
    float roughnessMapIntensity;
    float metallicMapIntensity;
    float normalMapIntensity;
    float emissiveMapIntensity;
    float aoMapIntensity;

    std::shared_ptr<Texture> colourMap;
    std::shared_ptr<Texture> roughnessMap;
    std::shared_ptr<Texture> metallicMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> emissiveMap;
    std::shared_ptr<Texture> aoMap;


    explicit Material(const glm::vec3& colour = glm::vec3(0.0f, 0.0f, 0.0f),
                      const float roughness = 0.5f,
                      const float metallic = 0.0f,
                      const float reflectivity = 0.0f,
                      const float iOR = 0.0f,
                      const float transparency = 0.0f,
                      const glm::vec3 attenuation = glm::vec3(0.0f))
        : colour(colour),
          roughness(roughness),
          metallic(metallic),
          reflectivity(reflectivity),
          iOR(iOR),
          transparency(transparency),
          attenuation(attenuation),
          emissive(0.0f),
          clearcoat(0.0f),
          clearcoatRoughness(0.0f),
          anisotropy(0.0f),
          anisotropyRotation(0.0f),
          sheen(0.0f),
          sheenColour(glm::vec3(1.0f)),
          colourMapIntensity(0.0f),
          roughnessMapIntensity(0.0f),
          metallicMapIntensity(0.0f),
          normalMapIntensity(0.0f),
          emissiveMapIntensity(0.0f),
          aoMapIntensity(0.0f)
    {
    }

    [[nodiscard]] glm::vec3 getColour(const glm::vec2& uv) const
    {
        const glm::vec3 base = colour;
        if (colourMap)
        {
            const glm::vec3 mapVal = colourMap->sample(uv.x, uv.y);
            return glm::mix(base, mapVal, colourMapIntensity);
        }
        return base;
    }

    [[nodiscard]] float getRoughness(const glm::vec2& uv) const
    {
        const float base = roughness;
        if (roughnessMap)
        {
            const float mapVal = roughnessMap->sample(uv.x, uv.y).r;
            return glm::mix(base, mapVal, roughnessMapIntensity);
        }
        return base;
    }

    [[nodiscard]] float getMetallic(const glm::vec2& uv) const
    {
        float base = metallic;
        if (metallicMap)
        {
            const float mapVal = metallicMap->sample(uv.x, uv.y).r;
            base = glm::mix(base, mapVal, metallicMapIntensity);
        }
        return base;
    }

    [[nodiscard]] glm::vec3 getNormal(const glm::vec2& uv, const glm::vec3 geomNormal) const
    {
        if (!normalMap) { return geomNormal; }
        glm::vec3 tangentNormal = normalMap->sample(uv.x, uv.y) * 2.0f - 1.0f;
        tangentNormal = glm::normalize(tangentNormal);
        return glm::normalize(geomNormal + tangentNormal * normalMapIntensity);
    }

    [[nodiscard]] glm::vec3 getEmissive(const glm::vec2& uv) const
    {
        glm::vec3 base = emissive;
        if (emissiveMap)
        {
            const glm::vec3 mapVal = emissiveMap->sample(uv.x, uv.y);
            base = glm::mix(base, mapVal, emissiveMapIntensity);
        }
        return base;
    }

    [[nodiscard]] float getAO(const glm::vec2& uv) const
    {
        float base = 1.0f;
        if (aoMap)
        {
            const float mapVal = aoMap->sample(uv.x, uv.y).r;
            base = glm::mix(base, mapVal, aoMapIntensity);
        }
        return base;
    }
};
