//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include <memory>
#include "BoundingBox.h"
#include "components/Material.h"
#include "../../rendering/structs/Intersection.h"
#include "../../rendering/structs/Ray.h"
#include "components/Transform.h"
#include "glm/gtx/associated_min_max.hpp"
#include <iostream>

struct Intersection;


class Entity {
        public:
        // Constructor
                Entity() = default;
                Entity(std::shared_ptr<Material> material) : material(std::move(material)) {}

        // Deconstructor
                virtual ~Entity() = default;

        //Key Methods
                virtual bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const = 0;

                [[nodiscard]] const Material& getMaterial() const { return *material; }

                void setMaterial(std::unique_ptr<Material>m) {material = std::move(m);}

                void setTransform (const Transform& t) {
                        std::cout << "setTransform called - position: ("
             << t.position.x << "," << t.position.y << "," << t.position.z << ")" << std::endl;
                        transform = t;
                }

                [[nodiscard]] const Transform& getTransform() const { return transform; }

                [[nodiscard]] Ray rayToObjectSpace(const Ray& ray) const {
                        const glm::mat4 inverse = transform.getInverseMatrix();
                        const auto origin = glm::vec3(inverse * glm::vec4(ray.origin,1.0f));
                        const auto direction = glm::vec3(inverse * glm::vec4(ray.direction,0.0f));
                        return Ray(origin, direction);
                }

                void objectIntersectionToWorldSpace(Intersection& hit) const {
                        const glm::mat4 matrix = transform.getMatrix();
                        hit.point = glm::vec3(matrix * glm::vec4(hit.point,1.0f));
                        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(matrix)));
                        hit.normal = glm::normalize(normalMatrix * hit.normal);
                }

                [[nodiscard]] virtual BoundingBox getBoundingBox() const = 0;

                [[nodiscard]] virtual bool isLight() const {return false;}

        // Copy Prevention
                Entity(const Entity&) = delete;
                Entity& operator=(const Entity&) = delete;

        protected:
                std::shared_ptr<const Material> material = std::make_shared<const Material>();
                Transform transform;
};


