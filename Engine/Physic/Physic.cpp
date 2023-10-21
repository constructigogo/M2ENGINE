//
// Created by Constantin on 04/08/2023.
//

#include "Physic.h"

namespace Engine {

    std::vector<CRigidBody *> Physic::bodies;
    float Physic::e = 1e-5;

    void Physic::init() {

    }

    void Physic::compute(double delta) {
        for (const auto body: bodies) {

        }
    }

    void Physic::registerBody(CRigidBody *body) {
        bodies.push_back(body);
    }

    bool Physic::raycast(const Ray &ray, RayHit &hit) {
        float t = 0.0;
        while (t < 100.0) {
            glm::vec3 point = ray.origin + ray.direction * t;
            float minDist = MAXFLOAT;
            float minDist2 = MAXFLOAT;
            float advance = minDist;
            for (const auto &rb: bodies) {
                if (rb->getImplCollider()) {
                    auto tr = rb->getObject()->getComponent<CTransform>();
                    auto mtx = tr->getTransformMTX();
                    glm::vec3 localPoint = glm::vec4(point,0.0) ;
                    const float dist2 = rb->getImplCollider()->dist2(localPoint);
                    if (dist2 < minDist2) {
                        minDist = rb->getImplCollider()->dist(localPoint);
                        advance = minDist;
                        minDist2 = dist2;
                        if (minDist < e) {
                            ENGINE_DEBUG("HIT");
                            hit.hitRef = rb->getObject();
                            hit.hitPosition = point;
                            return true;
                        }
                    }
                }
            }
            t += advance;
        }
        return false;
    }


} // Engine