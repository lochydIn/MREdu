//
// Created by Lochlan Harvey on 24/01/2026.
//

#pragma once
#include <memory>
#include "structs/BoundingBox.h"
#include "properties/Material.h"
#include "../../rendering/structs/Intersection.h"
#include "../../modelling/core/structs/Transform.h"
#include "../../rendering/structs/Ray.h"

struct Intersection;
struct Transform;

class Entity {
        public:
        // Constructor
                Entity() = default;
                explicit Entity(std::shared_ptr<Material> material) : material(std::move(material)) {}

        // Deconstructor
                virtual ~Entity() = default;

        //Key Methods
                virtual bool intersect(const Ray& ray, Intersection& hit, float tMin, float tMax) const = 0;

                [[nodiscard]] const Material& getMaterial() const { return *material; }

                static void setMaterial(std::unique_ptr<Material>material) {material = std::move(material);}

                void setTransform(const Transform& newTransform) { transform = newTransform; }

                [[nodiscard]] const Transform& getTransform() const { return transform; }

                [[nodiscard]] Ray worldToObject(const Ray& ray) const {
                        const glm::mat4 inv = transform.getInverseMatrix();
                        const auto origin = glm::vec3(inv * glm::vec4(ray.origin, 1.0f));
                        const auto direction = glm::vec3(inv * glm::vec4(ray.direction, 0.0f));
                        return Ray(origin, direction);
                }

                void objectToWorld(Intersection& hit) const {
                        const glm::mat4 matrix = transform.getMatrix();
                        hit.point = glm::vec3(matrix * glm::vec4(hit.point, 1.0f));
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


