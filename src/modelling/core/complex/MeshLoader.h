//
// Created by Lochlan Harvey on 10/03/2026.
//

#pragma once
#include <string>
#include <memory>
#include "glm/vec3.hpp"
class Mesh;
class Material;

Mesh* loadObjectMesh(const std::string& fileName,const glm::vec3& position, float scale, const std::shared_ptr<Material>& material);
