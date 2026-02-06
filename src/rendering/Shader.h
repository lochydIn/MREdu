//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "DirLight.h"
#include "Intersection.h"
#include "../modelling/components/Material.h"
#include "Ray.h"


class Shader
{
public:
        static glm::vec3 shade (const Material& material, const Intersection& hit,const DirLight& light, const Ray& ray);
};


