//
// Created by Lochlan Harvey on 25/01/2026.
//

#pragma once
#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../../../cmake-build-debug/_deps/glm-src/glm/mat4x4.hpp"
#include "../../../cmake-build-debug/_deps/glm-src/glm/vec3.hpp"
#include "../../../cmake-build-debug/_deps/glm-src/glm/gtx/matrix_transform_2d.hpp"

class Transform : public Component {
public:
    Transform() = default;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // Matrix
    glm::mat4x4 matrix = glm::mat4x4(1.0f);

    void update() override {
        // Moving.
        matrix =  glm::translate(matrix, position);
        // Rotating
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 1.0f, 0.0f));
        //Scaling
        matrix = glm::translate(matrix, scale);
    }
};

