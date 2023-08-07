//
// Created by Constantin on 04/08/2023.
//

#include "Physic.h"

namespace Engine {

    std::vector<CRigidBody*> Physic::bodies;

    void Physic::init() {

    }

    void Physic::compute(double delta) {
        for (const auto body: bodies) {
            body->computeAuxilaries();
            body->computeForces(delta);
            body->physicUpdate(delta);
        }
    }

    void Physic::registerBody(CRigidBody *body) {
        bodies.push_back(body);
    }


} // Engine