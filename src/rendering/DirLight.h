//
// Created by Lochlan Harvey on 04/02/2026.
//

#pragma once
#include "glm/vec3.hpp"

struct DirLight
{
public:
    glm::vec3 direction;
    glm::vec3 colour;
    float intensity;
};
