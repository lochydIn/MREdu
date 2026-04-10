//
// Created by Lochlan Harvey on 06/02/2026.
//

#include "RayTracer.h"

#include <iostream>

float RayTracer::halton(int index, const int base) {
    float result = 0.0f;
    float f = 1.0f;

    while (index > 0) {
        f /= base;
        result += f* (index % base);
        index = index / base;
    }
    return result;
}

glm::vec3 RayTracer::generateReflectionDirection(const glm::vec3& incident, const glm::vec3& normal,
    const float roughness, const int sampleIndex, const std::array<int, 2>& haltonBases) {
    // Perfect reflection direction
    const glm::vec3 perfectReflection = glm::reflect(incident, normal);

    // If no roughness, return perfect reflection
    if (roughness <= 0.0f) {
        return perfectReflection;
    }

    // Build tangent/bitangent basis for jitter
    glm::vec3 tangent = glm::normalize(glm::cross(
        normal,
        glm::abs(normal.x) > 0.9f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)
    ));
    glm::vec3 bitangent = glm::cross(normal, tangent);

    // Generate jitter using Halton sequence
    const float jitter1 = halton(sampleIndex, haltonBases[0]) * 2.0f - 1.0f;
    const float jitter2 = halton(sampleIndex, haltonBases[1]) * 2.0f - 1.0f;

    // Scale jitter by roughness (roughness^2 for better control)
    const float roughnessScale = roughness * roughness;
    const glm::vec3 perturbation = (tangent * jitter1 + bitangent * jitter2) * roughnessScale;

    // Add perturbation and normalize
    return glm::normalize(perfectReflection + perturbation);
}

glm::vec3 RayTracer::trace(const Scene& scene, const Ray& ray, const int depth,
    const int sampleIndex, const TraceParams& params) {
    static int rayCounter = 0;
    rayCounter++;

    // Return black - end Recursion
    if (depth > params.maxDepth) {
        return glm::vec3(0.0f);
    }

    // Randomly cull rays.
    if (params.russianRoulette && depth >= params.russianRouletteStartDepth) {
        float survival = glm::min(1.0f, params.russianRoulFactor);
        if (halton(sampleIndex + depth * 123, 5) > survival) {
            return glm::vec3(0.0f);
        }
    }

    const Intersection intersection = scene.intersect(ray);

    // Object infinitely far away or behind camera.
    if (intersection.distance < 0) {
        return params.backgroundColor;
    }

    glm::vec3 intersectionColour(0.0f);
    const Material& mat = intersection.entity->getMaterial();
    bool reflective = mat.reflectivity > 0.0f;
    bool refractive = mat.transparency > 0.0f;

    glm::vec3 incident = ray.direction;
    glm::vec3 hitPoint = intersection.point;
    glm::vec3 normal = intersection.normal;
    float epsilon = params.epsilon;

    // If transparent.
    if (refractive) {
        glm::vec3 refractionColour(0.0f);
        glm::vec3 reflectionColour(0.0f);
        // Fresnel Bit:
        float cosInternal = glm::clamp(glm::dot(incident,normal),-1.0f,1.0f);
        float iorI = 1; //The index of refraction for the incident medium.
        float iorT = mat.iOR; //The index of refraction for the transmitted medium.

        if (cosInternal > 0.0f) { std::swap(iorI,iorT);} // Swap values depending on entry or exit.
        float eta = iorI / iorT;

        float sinTransmitted = iorI / iorT * glm::sqrt(glm::max((1-cosInternal * cosInternal),0.0f));

        float kR = 0;
        if (sinTransmitted >= 1) { // Total internal reflection.
            kR = 1;
        } else {
            float cosTransmitted = glm::sqrt(glm::max((1 - sinTransmitted * sinTransmitted),0.0f));
            cosInternal = glm::abs(cosInternal);
            float  rS = ((iorT * cosInternal) - (iorI * cosTransmitted)) / ((iorT * cosInternal)+(iorI * cosTransmitted));
            float  rP = ((iorI * cosInternal)-(iorT * cosTransmitted)) / ((iorI * cosInternal)+(iorT * cosTransmitted));
            kR = (rS * rS + rP * rP) / 2;
        }
        bool outside = glm::dot(incident,normal) < 0;
        // Compute refraction if it is not a case of total internal reflection.

        if (kR < 1) { // if not a case of internal reflection.
            glm::vec3 refractionDirection = glm::refract(incident, normal, eta);
            glm::vec3 refractionRayOrigin = outside ? hitPoint - epsilon * normal : hitPoint + epsilon * normal;
            refractionColour = trace(scene,Ray(refractionRayOrigin,refractionDirection),depth+1,sampleIndex,params);


        }

        glm::vec3 reflectionRayDir = generateReflectionDirection(ray.direction,normal,
            mat.roughness,sampleIndex,params.haltonBases);
        glm::vec3 reflectionRayOrigin = outside ? hitPoint + epsilon * normal : hitPoint - epsilon * normal;
        reflectionColour = trace(scene,Ray(reflectionRayOrigin,reflectionRayDir),depth+1,sampleIndex,params);

        intersectionColour += reflectionColour * kR + refractionColour * (1 - kR);

    } else if (reflective) { // If just reflective.
        glm::vec3 directColour = Shader::shade(scene,intersection,ray,sampleIndex,params);


        glm::vec3 reflectionRayDir = generateReflectionDirection(ray.direction,normal,
            mat.roughness,sampleIndex,params.haltonBases);
        glm::vec3 reflectionRayOrigin = hitPoint + epsilon * normal;
        glm::vec3 reflectionColour = trace(scene,Ray(reflectionRayOrigin,reflectionRayDir),depth+1,sampleIndex,params);

        intersectionColour = directColour * 0.2f + reflectionColour * mat.colour * 0.8f;

    } else { // Opaque and Unreflective.
        intersectionColour = Shader::shade(scene,intersection,ray,sampleIndex,params);
    }
    return intersectionColour;
}


glm::vec3 RayTracer::tracePixelHalton(const Scene& scene, const Camera& camera,
    const float x, const float y, const RenderParams& params)
{
    glm::vec3 colour(0.0f);

    for (int i =0; i < params.primarySamples; i++)
    {
        const float jitterX = halton(i,2) - 0.5f;
        const float jitterY = halton(i,3) - 0.5f;

        const float sampleX = x + jitterX;
        const float sampleY = y + jitterY;

        Ray ray = camera.getRay(sampleX,sampleY);

        TraceParams traceParams;
        traceParams.maxDepth = params.maxDepth;
        traceParams.reflectionSamples = params.reflectionSamples;
        traceParams.shadowSamples = params.shadowSamples;
        traceParams.shadowBias = params.shadowBias;
        traceParams.shadowHaltonBases = params.haltonBases;
        traceParams.softShadows = params.softShadows;
        traceParams.backgroundColor = params.backgroundColor;
        traceParams.russianRoulette = params.russianRoulette;
        traceParams.russianRoulFactor = params.russianRoulFactor;
        traceParams.russianRouletteStartDepth = params.russianRouletteStartDepth;
        traceParams.reduceSamplesWithDepth = params.reduceSamplesWithDepth;
        traceParams.haltonBases = params.haltonBases;
        traceParams.epsilon = params.epsilon;
        traceParams.epsilonScale = params.epsilonScale;


        colour += trace(scene, ray, 0,i,traceParams);
    }
    return colour / static_cast<float>(params.primarySamples);
}

