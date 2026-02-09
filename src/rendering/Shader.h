//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "Intersection.h"
#include "../modelling/components/Material.h"
#include "Ray.h"


class Scene;

class Shader
{
public:
        static glm::vec3 shade (const Scene& scene, const Intersection& hit, const Ray& ray);
private:
        static float distrubutionGGX(float nDotH, float roughness);

        static float geometryShading(float nDotL, float nDotV, float roughness);

        static glm::vec3 fresnelSchlick(const Material& mat, float nDotV);
};


