//
// Created by Lochlan Harvey on 04/02/2026.
//

#include "Shader.h"
#include "glm/ext/scalar_constants.hpp"

// Main Shader function.
glm::vec3 Shader::shade (const Material& mat, const Intersection& hit, const DirLight& light, const Ray& ray) {

    glm::vec3 N = hit.normal;
    glm::vec3 L = -light.direction;
    glm::vec3 V = -ray.direction;
    glm::vec3 H = glm::normalize(L + V);

    float NdotL = glm::max(glm::dot(N,L),0.0f);
    float NdotV = glm::max(glm::dot(N,V),0.0f);
    float NdotH = glm::max(glm::dot(N,H),0.0f);

    glm::vec3 F0 = glm::mix(glm::vec3(0.04f), mat.colour, mat.metallic);
    // Fresnel Function.
    glm::vec3 F = F0 + (glm::vec3(1.0f) - F0) * glm::pow(1.0f - NdotV, 5.0f);

    float roughness2 = mat.roughness * mat.roughness;
    float roughness4 = roughness2 * roughness2;
    // Normal Distribution - GGX Trowbridge-Reitz
    float D = roughness4 / (glm::pi<float>() * pow(NdotH * NdotH * (roughness4 - 1.0f) + 1.0f, 2.0f));
    //
    float k = (mat.roughness + 1.0f) * (mat.roughness + 1.0f) / 8.0f;
    // Geometry Shading - Schlick GGX (Smith height/correlated masking-shadowing)
    float G = NdotV / (NdotV * (1.0f - k) + k) * NdotL / (NdotL * (1.0f - k) + k);

    // Cook Torrence Function / Specular Lighting
    glm::vec3 specular = (D * G * F) / (4.0f * NdotV * NdotL + 0.0001f);

    // Ks = Fresnel
    glm::vec3 kS = F;
    // kD = 1 - ks.
    glm::vec3 kD = (glm::vec3(1.0f) - kS) * (1.0f - mat.metallic);

    // Lambertian / Diffuse Lighting
    glm::vec3 diffuse = mat.colour / glm::pi<float>();

    // Final BDRF Shading function.
    return (kD * diffuse + specular) * light.colour * NdotL + mat.colour * 0.03f;

}




