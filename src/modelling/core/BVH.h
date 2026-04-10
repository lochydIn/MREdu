//
// Created by Lochlan Harvey on 19/02/2026.
//

#pragma once
#include <memory>
#include <vector>
#include "BoundingBox.h"
#include "Entity.h"
#include "primatives/Sphere.h"

struct BVHNode {
    BoundingBox box;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<Entity*> entities;
    bool isLeaf;

    BVHNode() : isLeaf (false) {}
};

class BVH {
    public:
        BVH(const std::vector<Entity*>& entities) : allEntities(entities) {
            if (!allEntities.empty()) {
                const std::vector<Entity*>& entitiesCopy = entities;
                root = buildBVH(entitiesCopy, 0);
            }
        }

        bool intersect(const Ray& ray, Intersection& hit) const {
            if (!root) return false;
            float tMin = 1e30f;
            return intersectNode(root.get(), ray, tMin, hit);
        }

    private:
        std::unique_ptr<BVHNode> root;
        std::vector<Entity*> allEntities;

        static std::unique_ptr<BVHNode> buildBVH(std::vector<Entity*> entities, const int depth) {
            auto node = std::make_unique<BVHNode>();

            // For all ents get bb.
            for (const auto ent : entities) {
                node->box.expand(ent->getBoundingBox());
            }
                // If leaf sized.
            if (entities.size() <= 4 || depth > 20) {
                node->isLeaf = true;
                node->entities = entities;
                return node;
            }

            int axis = node->box.maxDimension();

            std::sort(entities.begin(),entities.end(),[axis](const Entity* a, const Entity* b) {
                float centerA = (a->getBoundingBox().min[axis] + a->getBoundingBox().max[axis]) * 0.5f;
                float centerB = (b->getBoundingBox().min[axis] + b->getBoundingBox().max[axis]) * 0.5f;

                return centerA < centerB;
            });

            const size_t mid = entities.size() / 2;

            const std::vector entitiesLeft(entities.begin(), entities.begin() + mid);
            const std::vector entitiesRight(entities.begin() + mid, entities.end());

            node->left = buildBVH(entitiesLeft, depth + 1);
            node->right = buildBVH(entitiesRight, depth + 1);

            node->isLeaf = false;

            return node;
        }

        static bool intersectNode(const BVHNode* node, const Ray& ray, float& tMin, Intersection& hit)
        {
            float boxTMin, boxTMax;

            if (!node->box.intersect(ray,boxTMin, boxTMax)) {
                return false;
            }

            if (node->isLeaf) {
                bool bool_hit = false;

                for (const auto ent : node->entities) {
                Intersection tempHit;

                    if (ent->intersect(ray,tempHit,0.001f, tMin)) {
                        if (tempHit.distance < tMin && tempHit.distance > 0) {
                            tMin = tempHit.distance;
                            hit = tempHit;
                            bool_hit = true;
                        }

                    }
                }
                return bool_hit;
            }

            const bool hitLeft = intersectNode(node->left.get(), ray, tMin, hit);
            const bool hitRight = intersectNode(node->right.get(), ray, tMin, hit);

            return hitLeft || hitRight;
        }
};

