//
// Created by Lochlan Harvey on 06/02/2026.
//

#pragma once
#include <vector>
#include "../modelling/core/Scene.h"
#include "../modelling/core/primatives/Sphere.h"
#include "../modelling/core/lighting/DirectionalLight.h"
#include "structs/RenderParams.h"
#include "Shader.h"
#include "../modelling/core/primatives/Plane.h"
#include "../modelling/core/Camera.h"
#include "../modelling/core/primatives/Cuboid.h"


class RayTracer
{
    public:

    static glm::vec3 trace(const Scene& scene, const Ray& ray,
        int depth, int sampleIndex, const TraceParams& params);

    static glm::vec3 tracePixelHalton(const Scene& scene, const Camera& camera,
    float x, float y, const RenderParams& params);

    static glm::vec3 generateReflectionDirection(const glm::vec3& incident,const glm::vec3& normal,
        float roughness, int sampleIndex, const std::array<int, 2>& haltonBases);

    static float halton(int index, int base);
};


