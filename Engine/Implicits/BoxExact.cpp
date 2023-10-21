//
// Created by constantin on 18/10/23.
//

#include "BoxExact.h"

namespace Engine {
    float BoxExact::dist(const glm::vec3 &_p) const {
        glm::vec3 q = glm::abs(transPoint(_p));
        q = q - BoxDim;
        return length(glm::max(q, 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);
    }

    float BoxExact::dist2(const glm::vec3 &_p) const {
        float val = dist(_p);
        return val * val;
    }

    BoxExact::BoxExact(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &boxDim) : ImplicitTransform(pos,
                                                                                                                rot),
                                                                                              BoxDim(boxDim) {}
} // Engine