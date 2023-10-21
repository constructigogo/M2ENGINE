//
// Created by Constantin on 04/08/2023.
//

#ifndef ENGINE_CRIGIDBODY_H
#define ENGINE_CRIGIDBODY_H

#include "Component.h"
#include "CTransform.h"
#include "CMeshRenderer.h"
#include "../Physic/Physic.h"
#include "../Implicits/ImplicitAll.h"

namespace Engine {

    class Physic;

    class CRigidBody : public Component {
    public:

        CRigidBody();
        CRigidBody(Implicit * _collider);

        ~CRigidBody() override;

        void start() override;

        void fixedUpdate(double delta) override;

        void update(double delta) override;

        void lateUpdate(double delta) override;

        void addForce() {} //TODO

        bool affectedByGravity=true;

        Implicit *getImplCollider() const;

        void setImplCollider(Implicit *implCollider);

    private:
        void computeTensor();
        void computeAuxilaries ();
        void computeForces(double delta);

        void physicUpdate(double delta);

        CMeshRenderer* meshRenderer;
        Implicit* ImplCollider;

        /* Constant */
        float mass = 1;
        glm::mat3 tensor;    /* Ibody */
        glm::mat3 tensorInv; /* Inverses of Ibody */

        /* State variables */
        CTransform *transform; /* position : x(t) */ /* rotation : q(t) */
        glm::vec3 LinMomentum; /* P(t) */
        glm::vec3 AngMomentum; /* L(t) */

        /* Derived quantities (auxiliary variables) */
        glm::mat3 Iinv;     /* I−1(t) */
        glm::mat3 R;        /* R(t) */
        glm::vec3 velocity; /* v(t) */
        glm::vec3 angular;  /* w(t) */

        /* Computed quantities */
        glm::vec3 force;  /* F(t) */
        glm::vec3 torque; /* τ(t) */
    };

} // Engine

#endif //ENGINE_CRIGIDBODY_H
