//
// Created by Lochlan Harvey on 04/02/2026.
//

#include "Shader.h"

#include <iostream>

#include "../modelling/core/Scene.h"
#include "glm/ext/scalar_constants.hpp"

float Shader::distrubutionGGX(float nDotH, const float roughness) {
    float roughness2 = roughness * roughness;
    float roughness4 = roughness2 * roughness2;

    const float D = roughness4 / (glm::pi<float>() *
        glm::pow(nDotH * nDotH * (roughness4 - 1.0f) + 1.0f, 2.0f));
    return D;
}

float Shader::geometryShading(float const nDotL, const float nDotV, const float roughness) {
    float k = (roughness + 1.0f) * (roughness + 1.0f) / 8.0f;
    const float G = nDotV / (nDotV * (1.0f - k) + k) * nDotL / (nDotL * (1.0f - k) + k);
    return G;
}

glm::vec3 Shader::fresnelSchlick(const Material& mat, const float nDotV)
{
    const glm::vec3 F0 = glm::mix(glm::vec3(0.04f),mat.colour,mat.colour);
    // Fresnel Function.
    const glm::vec3 F = F0 + (glm::vec3(1.0f) - F0) * glm::pow(1.0f - nDotV, 5.0f);
    return F;
}


// Main Shader function.
glm::vec3 Shader::shade (const Scene& scene,
    const Intersection& hit, const Ray& ray) {

    glm::vec3 N = hit.normal;
    glm::vec3 V = -ray.direction;

    // Initial Ambient Light
    glm::vec3 ambient = hit.entity->getMaterial().colour * 0.3f;
    glm::vec3 totalColour = ambient;

    Material mat = hit.entity->getMaterial();

    for (const auto& light : scene.getLights())
    {
        glm::vec3 L = light->getDirection(hit.point);
        glm::vec3 H = glm::normalize(L + V);

        bool inShadow = false;
        glm::vec3 shadowOrigin = hit.point + N * 0.001f;
        Ray shadowRay(shadowOrigin, L);
        float dist = scene.intersect(shadowRay).distance;

        if (dist > 0 && dist < light->getDistance(hit.point)) {
            inShadow = true;
        }

        if (!inShadow) {
            float nDotL = glm::max(glm::dot(N,L),0.0f);
            float nDotV = glm::max(glm::dot(N,V),0.0f);
            float nDotH = glm::max(glm::dot(N,H),0.0f);

            // Normal Distribution - GGX Trowbridge-Reitz
            float D = distrubutionGGX(nDotH,mat.roughness);
            // Geometry Shading - Schlick GGX (Smith height/correlated masking-shadowing)
            float G = geometryShading(nDotL, nDotV, mat.roughness);
            // Fresnel Function.
            glm::vec3 F = fresnelSchlick(mat, nDotV);

            // Cook Torrence Function / Specular Lighting
            glm::vec3 specular = (D * G * F) / (4.0f * nDotV * nDotL + 0.0001f);

            // Ks = Fresnel
            glm::vec3 kS = F;
            // kD = 1 - ks.
            glm::vec3 kD = (glm::vec3(1.0f) - kS) * (1.0f - mat.metallic);

            // Lambertian / Diffuse Lighting
            glm::vec3 diffuse = mat.colour / glm::pi<float>();

            // Current lights contribution
            totalColour += (kD * diffuse + specular) * light->getColour() * nDotL;
        }
    }
    // Adding Emissive Light
    totalColour += mat.emissive;
    // Tone Mapping

    return totalColour;

}




