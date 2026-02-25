//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "structs/Intersection.h"
#include "structs/RenderParams.h"
#include "../modelling/core/components/Material.h"
#include "structs/Ray.h"
#include "../modelling/core/lighting/AreaLight.h"

class Scene;

class Shader
{
public:
        static glm::vec3 shade (const Scene& scene, const Intersection& intersection,
                const Ray& ray, int sampleIndex, const TraceParams& params);

private:
        static float distributionGGX(float nDotH, float roughness);

        static float geometryShading(float nDotL, float nDotV, float roughness);

        static glm::vec3 fresnelSchlick(const Material& mat, float nDotV);

        static glm::vec3 bRDF(const Material& mat, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L);


};


