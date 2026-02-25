//
// Created by Lochlan Harvey on 04/02/2026.
//

#include "Shader.h"

#include <iostream>
#include <cmath>
#include "RayTracer.h"
#include "../modelling/core/Scene.h"
#include "../modelling/core/lighting/PointLight.h"
#include "glm/ext/quaternion_exponential.hpp"
#include "glm/ext/scalar_constants.hpp"

float Shader::distributionGGX(float nDotH, const float roughness) {
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
    const glm::vec3 F0 = glm::mix(glm::vec3(0.04f),mat.colour,mat.metallic);
    // Fresnel Function.
    const glm::vec3 F = F0 + (glm::vec3(1.0f) - F0) * glm::pow(1.0f - nDotV, 5.0f);
    return F;
}

glm::vec3 Shader::bRDF(const Material& mat, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L) {

    glm::vec3 H = glm::normalize(V + L);
    float nDotV = glm::max(glm::dot(N,V),0.0001f);
    float nDotL = glm::max(glm::dot(N,L),0.0001f);
    float nDotH = glm::max(glm::dot(N,H),0.0001f);


    glm::vec3 F = fresnelSchlick(mat,nDotV);
    float D = distributionGGX(nDotH, mat.roughness);
    float G = geometryShading(nDotL, nDotV, mat.roughness);

    // Specular BRDF
    glm::vec3 specular = (D * G * F) / (4.0f * nDotV * nDotL);

    // Diffuse BRDF (lambertian)
    glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - mat.metallic);
    glm::vec3 diffuse = kD * mat.colour / glm::pi<float>();

    return diffuse + specular;
}

glm::vec3 jitterDirection(const glm::vec3& baseDir, float coneAngle, int sampleIndex,
    const std::array<int, 2>& haltonBases) {
    glm::vec3 normalisedBaseDir = glm::normalize(baseDir);

    if (coneAngle <= 0.0f) {
        return normalisedBaseDir;
    }

    const float r1 = RayTracer::halton(sampleIndex,haltonBases[0]);
    const float r2 = RayTracer::halton(sampleIndex,haltonBases[1]);

    const float cosTheta = glm::mix(1.0f,glm::cos(coneAngle),r1);
    const float sinTheta = glm::sqrt(glm::max(0.0f,1.0f - cosTheta * cosTheta));
    const float phi = r2 * 2.0f * glm::pi<float>();

    const glm::vec3 localJitter(
        sinTheta * glm::cos(phi),
        sinTheta * glm::sin(phi),
        cosTheta);

    const glm::vec3 up = glm::abs(normalisedBaseDir.y) < 0.999f
        ? glm::vec3(0.0f, 1.0f, 0.0f)
        : glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 tangent = glm::normalize(glm::cross(normalisedBaseDir,up));
    const glm::vec3 bitangent = glm::cross(normalisedBaseDir,tangent);

    glm::vec3 result = tangent * localJitter.x +
                      bitangent * localJitter.y +
                      normalisedBaseDir * localJitter.z;
    return glm::normalize(result);
}

// Main Shader function.
glm::vec3 Shader::shade (const Scene& scene, const Intersection& intersection, const Ray& ray,
    int sampleIndex, const TraceParams& params) {

    const Material& mat = intersection.entity->getMaterial();
    glm::vec3 N = glm::normalize(intersection.normal);
    glm::vec3 V = -glm::normalize(ray.direction);
    glm::vec3 colour = mat.colour * 0.1f; //Ambient Light

    for (const auto& light : scene.getLights()) {

        if (auto pointLight = dynamic_cast<PointLight*>(light)) {
            glm::vec3 lightPos = pointLight->getPosition();
            glm::vec3 lightColour = pointLight->getColour();

            if (params.softShadows && params.shadowSamples > 1 && pointLight->getRadius() > 0) {
                glm::vec3 totalLight(0.0f);

                for (int i = 0; i < params.shadowSamples; i++) {
                    float jitter1 = RayTracer::halton(sampleIndex * params.shadowSamples + i,
                        params.shadowHaltonBases[0]) * 2.0f - 1.0f;
                    float jitter2 = RayTracer::halton(sampleIndex * params.shadowSamples + i,
                        params.shadowHaltonBases[1]) * 2.0f - 1.0f;

                    glm::vec3 toLight = lightPos - intersection.point;
                    glm::vec3 lightDir = glm::normalize(toLight);

                    glm::vec3 tangent = glm::normalize(glm::cross(lightDir,
                        glm::abs(lightDir.x) > 0.9f ? glm::vec3(0,1,0) : glm::vec3(1,0,0)
                        ));
                    glm::vec3 bitangent = glm::normalize(glm::cross(lightDir,tangent));

                    glm::vec3 jitteredLightPos = lightPos + (tangent * jitter1 + bitangent * jitter2)
                        * pointLight->getRadius();

                    glm::vec3 toJitteredLight = jitteredLightPos - intersection.point;
                    float distance = glm::length(toJitteredLight);
                    glm::vec3 L = toJitteredLight/distance;

                    float nDotL = glm::max(glm::dot(N,L),0.0f);

                    if (nDotL > 0.0f) {
                        glm::vec3 shadowOrigin = intersection.point + N * params.shadowBias;
                        Ray shadowRay(shadowOrigin, L);
                        Intersection shadowHit = scene.intersect(shadowRay);

                        if (shadowHit.distance < 0 || shadowHit.distance > distance) {
                            glm::vec3 brdfValue = bRDF(mat,N,V,L);
                            float attenuation = 1.0f / (distance * distance);
                            totalLight += brdfValue * lightColour * nDotL * attenuation;
                        }
                    }
                }
                colour += totalLight / static_cast<float>(params.shadowSamples);

            } else {
                glm::vec3 toLight = lightPos - intersection.point;
                float distance = glm::length(toLight);
                glm::vec3 L = toLight / distance;

                float nDotL = glm::max(glm::dot(N,L),0.0f);
                if (nDotL > 0.0f) {
                    glm::vec3 shadowOrigin = intersection.point + N * params.shadowBias;
                    Ray shadowRay(shadowOrigin, L);
                    Intersection shadowHit = scene.intersect(shadowRay);

                    if (shadowHit.distance < 0 || shadowHit.distance > distance)
                    {
                        glm::vec3 brdfValue = bRDF(mat,N,V,L);
                        float attenuation = 1.0f / (distance * distance);
                        colour += brdfValue * lightColour * nDotL * attenuation;
                    }
                }
            }
        } else if (auto directionalLight = dynamic_cast<DirectionalLight*>(light)) {
            glm::vec3 baseLightDir = glm::normalize(directionalLight->getDirection());
            float const lightAngle = directionalLight->getAngle();

            if (params.softShadows && params.shadowSamples > 1 && lightAngle > 0) {
                glm::vec3 totalLight(0.0f);

                for (int i = 0; i < params.shadowSamples; i++) {

                    glm::vec3 L = jitterDirection(baseLightDir,lightAngle,
                        sampleIndex * params.shadowSamples + i, params.shadowHaltonBases);
                    float nDotL = glm::max(glm::dot(N,L),0.0f);

                    if (nDotL > 0.0f) {
                        glm::vec3 shadowOrigin = intersection.point + N * params.shadowBias;
                        Ray shadowRay(shadowOrigin, L);
                        Intersection shadowHit = scene.intersect(shadowRay);

                        if (shadowHit.distance < 0) {
                            glm::vec3 brdfValue = bRDF(mat,N,V,L);
                            totalLight += brdfValue * directionalLight->getColour() * nDotL;
                        }
                    }
                }
                colour += totalLight / static_cast<float>(params.shadowSamples);
            } else {
                glm::vec3 L = baseLightDir;
                float nDotL = glm::max(glm::dot(N,L),0.0f);
                if (nDotL > 0.0f)
                {
                    glm::vec3 shadowOrigin = intersection.point + N * params.shadowBias;
                    Ray shadowRay(shadowOrigin, L);
                    Intersection shadowHit = scene.intersect(shadowRay);
                    if (shadowHit.distance < 0)
                    {
                        glm::vec3 brdfValue = bRDF(mat,N,V,L);
                        colour += brdfValue * directionalLight->getColour() * nDotL;
                    }
                }
            }

        } else if (auto areaLight = dynamic_cast<AreaLight*>(light)) {
            glm::vec3 totalLight(0.0f);

            for (int i = 0; i < params.shadowSamples; i++) {
                float r1 = RayTracer::halton(
                    sampleIndex * params.shadowSamples + i, params.shadowHaltonBases[0]);

                float r2 = RayTracer::halton(
                    sampleIndex * params.shadowSamples + i, params.shadowHaltonBases[1]);

                LightSample sample = areaLight->sample(intersection.point,r1,r2);

                float cosTheta = glm::max(glm::dot(N,sample.w),0.0f);
                float cosThetaLight = glm::max(glm::dot(sample.lN,-sample.w),0.0f);

                if (cosTheta > 0.0f && cosThetaLight > 0.0f) {
                    glm::vec3 shadowOrigin = intersection.point + N * params.shadowBias;
                    Ray shadowRay(shadowOrigin,sample.w);
                    Intersection shadowHit = scene.intersect(shadowRay);
                    if (shadowHit.distance < 0 || shadowHit.distance > sample.distance) {
                        glm::vec3 brdfValue = bRDF(mat,N,V,sample.w);
                        float attenuation = 1.0f / sample.distance * sample.distance;
                        totalLight +=
                            brdfValue * sample.Le * cosThetaLight * attenuation / sample.pdf;
                    }
                }
            }
            colour += totalLight / static_cast<float>(params.shadowSamples);
        }
    }
    colour += mat.emissive;
    colour = glm::clamp(colour, 0.0f, 1.0f);
    return colour;

};




