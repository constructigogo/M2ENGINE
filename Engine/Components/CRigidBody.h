//
// Created by Constantin on 04/08/2023.
//

#ifndef ENGINE_CRIGIDBODY_H
#define ENGINE_CRIGIDBODY_H

#include "Component.h"
#include "CTransform.h"
#include "CMeshRenderer.h"

namespace Engine {

    class CRigidBody : public Component {
    public:
        void start() override;

        void fixedUpdate(double delta) override;

        void update(double delta) override;

        void lateUpdate(double delta) override;

    private:
        void computeTensor();


        CTransform * transform;
        CMeshRenderer * meshRenderer;

        float mass=1;
        glm::vec3 speed;
        glm::vec3 momentum;
        glm::vec3 kinetic;
        glm::vec3 force;
        glm::vec3 angular;
        glm::vec3 torque;
        glm::mat3 tensor;
    };

} // Engine

#endif //ENGINE_CRIGIDBODY_H
