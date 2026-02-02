//
// Created by Lochlan Harvey on 25/01/2026.
//
#pragma once
#include "../modelling/components/Component.h"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera {
public:
    // Constructor - Being passed the position of the camera and its target view point.
    explicit Camera(const glm::vec3& m_position = glm::vec3(0.0f, 0.0f, 5.0f),
                    const glm::vec3& m_target = glm::vec3(0.0f, 0.0f, 0.0f),
                    const float m_fov = 60.0f,
                    const float m_width = 480.0f,
                    const float m_height = 360.0f
                    )
        :
        fov(m_fov),
        aspect(m_width / m_height),
        height(m_height),
        width(m_width),
        scale(tan(glm::radians(fov * 0.5f))),
        position(m_position),
        target(m_target) {}


    // Deconstructor
    virtual ~Camera() = default;


    // Get the view matrix for the camera.
    [[nodiscard]] glm::mat4x4 getViewMatrix() const {
        return glm::lookAt(position,target,glm::vec3(0.0f,1.0f,0.0f));
    }
    // Get the ray from the camera's position to the given pixel u,v.
    Ray getRay(const int& px, const int& py) const {

        float u = (2.0f * px / width) - 1.0f;
        float v = -(2.0f * py / height) + 1.0f;

        const glm::vec3 forward = glm::normalize(target - position);
        const glm::vec3 right = glm::vec3(1.0f,0.0f,0.0f);
        const glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);

        //glm::vec3 rayDirection = glm::normalize(forward + (u * scale * right) + (v * scale * up));
        glm::vec3 rayDirection = glm::normalize(forward + (u * scale * aspect * right) + (v * scale * up));
        return Ray(position,rayDirection);
    }

    // Getters & Setters
    void setPosition(const glm::vec3& m_position) {
        position = m_position;
    }

    glm::vec3 getPosition() const {
        return position;
    }
    void setTarget(const glm::vec3& m_view) {
        target = m_view;
    }

    glm::vec3 getTarget() const {
        return target;
    }

    void setFov(const float& m_fov) {
        fov = m_fov;
    }

    float getFov() const {
        return fov;
    }


private:
    float fov;
    float aspect;
    float scale;
    float width;
    float height;
    glm::vec3 position;
    glm::vec3 target;
};

