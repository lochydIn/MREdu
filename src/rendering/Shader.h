//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "structs/Intersection.h"
#include "structs/RenderParams.h"
#include "../modelling/core/components/Material.h"
#include "structs/Ray.h"

class Scene;

class Shader
{
public:
    static glm::vec3 shade(const Scene& scene, const Intersection& hit,
                           const Ray& ray, int sampleIndex, const TraceParams& params);

private:
    static float distributionGGX(float nDotH, float hDotT, float hDotB,
                                 float ax, float ay);

    static float distributionGGXIsotropic(float nDotH, float roughness);

    static float geometryShading(float nDotL, float nDotV, float roughness);

    static glm::vec3 fresnelSchlick(const glm::vec3& surfaceColour, float metallic, float reflective, float nDotV);

    static glm::vec3 bRDF(glm::vec3 surfaceColour, float roughness, float metallic, float reflective,
                          float clearcoat, float clearcoatRoughness,
                          float sheen, glm::vec3 sheenColour,
                          float anisotropy, float anisotropyRotation, glm::vec3 T, glm::vec3 B,
                          const glm::vec3& N, const glm::vec3& V, const glm::vec3& L);
};