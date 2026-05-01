//
// Created by Lochlan Harvey on 10/03/2026.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../../tiny_obj_loader.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include <iostream>
#include "../primatives/Triangle.h"

Mesh* loadObjectMesh(const std::string& fileName, const std::shared_ptr<Material>& material)
{
    tinyobj::attrib_t attr;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    bool success = tinyobj::LoadObj(&attr, &shapes, &materials, &warning, &error, fileName.c_str());

    if (!success)
    {
        std::cerr << "Failed to load object: " << warning << std::endl;
        return nullptr;
    }

    std::cout << "Vertices: " << attr.vertices.size() / 3 << std::endl;
    std::cout << "Normals: " << attr.normals.size() / 3 << std::endl;
    std::cout << "UVs: " << attr.texcoords.size() / 2 << std::endl;

    std::vector<Triangle*> triangles;
    bool hasNormals = !attr.normals.empty();
    bool hasUVs = !attr.texcoords.empty();

    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            unsigned int fv = shape.mesh.num_face_vertices[f];
            std::vector<glm::vec3> faceVertices;
            std::vector<glm::vec3> faceNormals;
            std::vector<glm::vec2> faceUVs;

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                //Position
                float vx = attr.vertices[3 * idx.vertex_index + 0] * 40.0f;
                float vy = attr.vertices[3 * idx.vertex_index + 1] * 40.0f;
                float vz = attr.vertices[3 * idx.vertex_index + 2] * 40.0f;
                auto vertex = glm::vec3(vx, vy, vz);
                faceVertices.emplace_back(vertex);
                //Vertex Normals
                if (hasNormals && idx.normal_index >= 0)
                {
                    float nx = attr.normals[3 * idx.normal_index + 0];
                    float ny = attr.normals[3 * idx.normal_index + 1];
                    float nz = attr.normals[3 * idx.normal_index + 2];
                    faceNormals.emplace_back(nx, ny, nz);
                }
                //UVs
                if (hasUVs && idx.texcoord_index >= 0)
                {
                    float tx = attr.texcoords[2 * idx.texcoord_index + 0];
                    float ty = attr.texcoords[2 * idx.texcoord_index + 1];
                    faceUVs.emplace_back(tx, ty);
                }
                else
                {
                    glm::vec3 normal = glm::normalize(vertex);
                    float m_U = 0.5f + atan2(normal.z, normal.x) / (2.0 * M_PI);
                    float m_V = 0.5f - asin(normal.y) / M_PI;
                    faceUVs.emplace_back(m_U, m_V);
                }
            }

            if (fv == 3)
            {
                if (faceNormals.size() == 3 && faceUVs.size() == 3)
                {
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormals[0], faceNormals[1], faceNormals[2],
                        faceUVs[0], faceUVs[1], faceUVs[2],
                        material));
                }
                else if (faceNormals.size() == 3 && faceUVs.empty())
                {
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormals[0], faceNormals[1], faceNormals[2],
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material));
                }
                else if (faceNormals.empty() && faceUVs.size() == 3)
                {
                    glm::vec3 faceNormal = glm::normalize(glm::cross(faceVertices[1] - faceVertices[0],
                                                                     faceVertices[2] - faceVertices[0]));
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormal, faceNormal, faceNormal,
                        faceUVs[0], faceUVs[1], faceUVs[2],
                        material));
                }
                else
                {
                    glm::vec3 faceNormal = glm::normalize(glm::cross(faceVertices[1] - faceVertices[0],
                                                                     faceVertices[2] - faceVertices[0]));
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormal, faceNormal, faceNormal,
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material));
                }
            }
            else if (fv == 4)
            {
                if (faceNormals.size() == 4 && faceUVs.size() == 4)
                {
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormals[0], faceNormals[1], faceNormals[2],
                        faceUVs[0], faceUVs[1], faceUVs[2],
                        material));

                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[2], faceVertices[3],
                        faceNormals[0], faceNormals[2], faceNormals[3],
                        faceUVs[0], faceUVs[2], faceUVs[3],
                        material));
                }
                else if (faceNormals.size() == 4 && faceUVs.empty())
                {
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormals[0], faceNormals[1], faceNormals[2],
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material));

                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[2], faceVertices[3],
                        faceNormals[0], faceNormals[2], faceNormals[3],
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material));
                }
                else if (faceNormals.empty() && faceUVs.size() == 4)
                {
                    glm::vec3 faceNormal1 = glm::normalize(glm::cross(
                        faceVertices[1] - faceVertices[0],
                        faceVertices[2] - faceVertices[0]
                    ));
                    glm::vec3 faceNormal2 = glm::normalize(glm::cross(
                        faceVertices[2] - faceVertices[0],
                        faceVertices[3] - faceVertices[0]
                    ));
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormal1, faceNormal1, faceNormal1,
                        faceUVs[0], faceUVs[1], faceUVs[2],
                        material
                    ));
                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[2], faceVertices[3],
                        faceNormal2, faceNormal2, faceNormal2,
                        faceUVs[0], faceUVs[2], faceUVs[3],
                        material));
                }
                else
                {
                    glm::vec3 faceNormal1 = glm::normalize(glm::cross(
                        faceVertices[1] - faceVertices[0],
                        faceVertices[2] - faceVertices[0]
                    ));
                    glm::vec3 faceNormal2 = glm::normalize(glm::cross(
                        faceVertices[2] - faceVertices[0],
                        faceVertices[3] - faceVertices[0]
                    ));

                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[1], faceVertices[2],
                        faceNormal1, faceNormal1, faceNormal1,
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material
                    ));

                    triangles.push_back(new Triangle(
                        faceVertices[0], faceVertices[2], faceVertices[3],
                        faceNormal2, faceNormal2, faceNormal2,
                        glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0),
                        material
                    ));
                }
            }


            index_offset += fv;
        }
    }
    std::cout << "Loaded " << triangles.size() << " triangles from" << fileName << std::endl;
    return new Mesh(triangles, material);
}
