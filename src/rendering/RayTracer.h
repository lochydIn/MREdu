//
// Created by Lochlan Harvey on 06/02/2026.
//

#pragma once
#include <vector>
#include "../modelling/core/Scene.h"
#include "../modelling/core/primatives/Sphere.h"
#include "../modelling/core/lighting/DirectionalLight.h"
#include "Shader.h"
#include "../modelling/core/primatives/Plane.h"


class RayTracer
{
    public:

    static glm::vec3 trace(const Scene& scene,const Ray& ray, int depth);
};


