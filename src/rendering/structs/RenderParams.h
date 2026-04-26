//
// Created by Lochlan Harvey on 17/02/2026.
//

#pragma once

#include <glm/glm.hpp>
#include <array>

struct RenderParams
{
    int primarySamples = 4;
    int reflectionSamples = 4;
    int shadowSamples = 4;
    bool softShadows = false;
    float shadowBias = 0.001f;
    std::array<int, 2> shadowHaltonBases = {5, 7};
    int maxDepth = 3;
    float jitter = 1.0f;
    glm::vec3 backgroundColor = glm::vec3(0.4);
    bool russianRoulette = true;
    float russianRoulFactor = 0.5f;
    int russianRouletteStartDepth = 2;
    bool reduceSamplesWithDepth = false;
    std::array<int, 2> haltonBases = {2, 3};
    float epsilon = 0.001f;
    float epsilonScale = 1e-4f;
};

struct TraceParams
{
    int primarySamples;
    int reflectionSamples;
    int shadowSamples;
    bool softShadows;
    float shadowBias;
    std::array<int, 2> shadowHaltonBases;
    int maxDepth;
    glm::vec3 backgroundColor;
    bool russianRoulette;
    float russianRoulFactor;
    int russianRouletteStartDepth;
    bool reduceSamplesWithDepth;
    std::array<int, 2> haltonBases;
    float epsilon;
    float epsilonScale;
};