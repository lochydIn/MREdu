//
// Created by Lochlan Harvey on 06/02/2026.
//

#include "RayTracer.h"

#include <chrono>
#include <iostream>



glm::vec3 RayTracer::trace(const Scene& scene, const Ray& ray, int depth) {
    if (depth > 5) {
        return glm::vec3(0,0,0);
    }

    Intersection hit = scene.intersect(ray);
    if (hit.distance < 0) {
        return glm::vec3(0.67,0.847,0.902);
    }

    glm::vec3 localColour = Shader::shade(scene,hit,ray);

    if (hit.entity->getMaterial().reflectivity > 0) {

        glm::vec3 incident = ray.direction;

        glm::vec3 reflectedDir = glm::reflect(incident,hit.normal);

        glm::vec3 rayStart = hit.point + hit.normal * 0.01f;

        Ray reflectedRay = Ray(rayStart,reflectedDir);

        glm::vec3 reflectedColour = trace(scene, reflectedRay,depth + 1);

        localColour = glm::mix(localColour,reflectedColour,hit.entity->getMaterial().reflectivity);
    }

    return localColour;
}

