//
// Created by Lochlan Harvey on 10/03/2026.
//

#pragma once
#include <string>
#include <memory>

class Mesh;
class Material;

Mesh* loadObjectMesh(const std::string& fileName, const std::shared_ptr<Material>& material);
