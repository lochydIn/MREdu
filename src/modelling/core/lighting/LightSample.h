//
// Created by Lochlan Harvey on 19/02/2026.
//

#pragma once

struct LightSample {
    glm::vec3 Le;   // Emitted radiance.
    glm::vec3 w;    // Direction to light (normalized).
    float pdf;      // Probability density (in area or solid angle measure).
    float distance; // Distance to light from point.
    glm::vec3 lN;   // Normal at sampled point.
    glm::vec3 xL;    // The actual point on light (sometimes needed).
};