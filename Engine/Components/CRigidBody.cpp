//
// Created by Constantin on 04/08/2023.
//

#include <iostream>
#include <unordered_set>
#include "CRigidBody.h"
#include "../Log.h"

namespace Engine {
    void CRigidBody::start() {
        transform = getParent()->getComponent<CTransform>();
        if (!transform) {
            transform = getParent()->addComponent<CTransform>();
        }

        meshRenderer = getParent()->getComponent<CMeshRenderer>();
        computeTensor();
    }

    void CRigidBody::fixedUpdate(double delta) {

    }

    void CRigidBody::update(double delta) {

    }

    void CRigidBody::lateUpdate(double delta) {

    }

    void CRigidBody::computeTensor() {
        auto &bxpos = transform->getPosition();
        tensor = glm::zero<glm::mat3>();
        std::vector<glm::vec3> data;
        int i=0;
        glm::vec3 r(bxpos.x, bxpos.y, bxpos.z);
        for (const auto &mesh: meshRenderer->mesh->subMeshes) {
            for (const auto &vert: mesh.vertexesData) {
                bool skip = false;
                for (const auto & used: data) {
                    if (used == vert.position){
                        skip =true;
                        break;
                    }
                }
                if (skip){
                    continue;
                }
                data.push_back(vert.position);
                glm::vec3 r1 = vert.position - r;
                float v1 = (r.x * r.x + r.y * r.y + r.z * r.z);
                auto v2 = glm::identity<glm::mat3>();
                glm::mat3 v3(
                        r.x * r.x, r.x * r.y, r.x * r.z,
                        r.y * r.x, r1.y * r1.y, r.y * r.z,
                        r.z * r.x, r1.z * r.y, r.z * r.z
                );

                tensor = tensor + ((v1 * v2) - v3);
            }
        }
        std::cout << tensor[0][0] << " " << tensor[0][1] << " " << tensor[0][2]<< " " << std::endl;
        std::cout << tensor[1][0] << " " << tensor[1][1] << " " << tensor[1][2]<< " " << std::endl;
        std::cout << tensor[2][0] << " " << tensor[2][1] << " " << tensor[2][2]<< " " << std::endl;
    }
} // Engine