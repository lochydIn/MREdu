//
// Created by Lochlan Harvey on 25/01/2026.
//
#pragma once
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

class Camera
{
public:
    // Constructor - Being passed the position of the camera and its target view point.
    explicit Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 5.0f),
                    const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f),
                    const float fov = 60.0f,
                    const float width = 960.0f,
                    const float height = 720.0f
    )
        :
        fov(fov),
        aspect(width / height),
        scale(tan(glm::radians(fov * 0.5f))),
        width(width),
        height(height),
        position(position),
        target(target)
    {
    }


    // Deconstructor
    virtual ~Camera() = default;


    // Get the view matrix for the camera.
    [[nodiscard]] glm::mat4x4 getViewMatrix() const
    {
        return glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Get the ray from the camera's position to the given pixel u,v.
    [[nodiscard]] Ray getRay(const int& px, const int& py) const
    {
        const float u = (2.0f * px / width) - 1.0f;
        const float v = 1.0f - (2.0f * py / height);

        const glm::vec3 forward = glm::normalize(target - position);
        constexpr auto right = glm::vec3(1.0f, 0.0f, 0.0f);
        constexpr auto up = glm::vec3(0.0f, 1.0f, 0.0f);

        const glm::vec3 rayDirection = glm::normalize(forward + (u * scale * aspect * right) + (v * scale * up));
        return Ray(position, rayDirection);
    }

    // Getters & Setters
    void setPosition(const glm::vec3& m_position)
    {
        position = m_position;
    }

    [[nodiscard]] glm::vec3 getPosition() const
    {
        return position;
    }

    void setTarget(const glm::vec3& m_view)
    {
        target = m_view;
    }

    [[nodiscard]] glm::vec3 getTarget() const
    {
        return target;
    }

    void setFov(const float& m_fov)
    {
        fov = m_fov;
    }

    [[nodiscard]] float getFov() const
    {
        return fov;
    }

    void setAspect(const float& aspectRatio, int m_width, int m_height)
    {
        aspect = aspectRatio;
        width = m_width;
        height = m_height;
    }

    [[nodiscard]] glm::vec3 getForward() const
    {
        return glm::normalize(target - position);
    }

    [[nodiscard]] glm::vec3 getRight() const
    {
        return glm::normalize(glm::cross(getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    void moveZ(const float amount)
    {
        position += getForward() * amount;
        target = getForward() * amount;
    }

    void moveX(const float amount)
    {
        position += getRight() * amount;
        target = getRight() * amount;
    }

    void moveY(const float amount)
    {
        position.y += amount;
        target.y += amount;
    }

    void look(const float yaw, float const pitch)
    {
        glm::vec3 direction = target - position;
        glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        direction = glm::vec3(yawMatrix * glm::vec4(direction, 0.0f));

        glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), pitch, right);
        direction = glm::vec3(pitchMatrix * glm::vec4(direction, 0.0f));
        target = position + direction;
        std::cout << "New target: (" << target.x << ", " << target.y << ", " << target.z << ")" << std::endl;
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