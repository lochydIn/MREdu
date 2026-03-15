//
// Created by Lochlan Harvey on 10/03/2026.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../tiny_obj_loader.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include <iostream>
#include "../primatives/Triangle.h"

Mesh* loadObjectMesh(const std::string& fileName, const glm::vec3& position, const std::shared_ptr<Material>& material) {
    tinyobj::attrib_t attr;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    bool success = tinyobj::LoadObj(&attr,&shapes,&materials,&warning,&error,fileName.c_str());

    if(!success) {
        std::cerr << "Failed to load object: " << warning << std::endl;
        return nullptr;
    }

    std::vector<Triangle*> triangles;

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            unsigned int fv = shape.mesh.num_face_vertices[f];
            std::vector<glm::vec3> faceVertices;

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                float vx = attr.vertices[3 * idx.vertex_index + 0];
                float vy = attr.vertices[3 * idx.vertex_index + 1];
                float vz = attr.vertices[3 * idx.vertex_index + 2];

                faceVertices.emplace_back(vx, vy, vz);
            }

            if (fv == 3) {
                triangles.emplace_back(new Triangle(faceVertices[0],faceVertices[1],faceVertices[2],material));
            }
            if (fv == 4) {
                triangles.emplace_back(new Triangle(faceVertices[0],faceVertices[1],faceVertices[2],material));
                triangles.emplace_back(new Triangle(faceVertices[0],faceVertices[2],faceVertices[3],material));
            }
            index_offset += fv;
        }
    }
    std::cout << "Loaded " << triangles.size() << " triangles from" << fileName << std::endl;
    return new Mesh(position,triangles,material);
}