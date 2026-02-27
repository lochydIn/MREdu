//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "Component.h"
#include "glm/vec3.hpp"

class Material : public Component {

    public:

        glm::vec3 colour;
        float roughness;
        float metallic;
        glm::vec3 emissive;
        float reflectivity = 0.0f;
        float iOR;
        float transparency;
        glm::vec3 attenuation;

        explicit Material(const glm::vec3& colour = glm::vec3(0.0f,0.0f,0.0f),
                          const float roughness = 0.5f,
                          const float metallic = 0.0f,
                          const float reflectivity = 0.0f,
                          const float iOR = 0.0f,
                          const float transparency = 0.0f,
                          const glm::vec3 attenuation = glm::vec3(0.0f,0.0f,0.0f))
            : colour(colour),
            roughness(roughness),
            metallic(metallic),
            emissive(0.0f,0.0f,0.0f),
            reflectivity(reflectivity),
            iOR(iOR),
            transparency(transparency),
            attenuation(attenuation){}
};
