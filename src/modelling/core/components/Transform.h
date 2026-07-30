//
// Created by Lochlan Harvey on 25/01/2026.
//

#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    [[nodiscard]] glm::mat4 getMatrix() const
    {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, scale);
        return matrix;
    }

    [[nodiscard]] glm::mat4 getInverseMatrix() const
    {
        return glm::inverse(getMatrix());
    }
};
