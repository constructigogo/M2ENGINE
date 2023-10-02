//
// Created by constantin on 28/09/23.
//

#include "Difference.h"

namespace Engine {
    float Difference::dist(const glm::vec3 &p) const {
        return std::max(l->dist(p),-r->dist(p));
    }

    float Difference::dist2(const glm::vec3 &p) const {
        return std::max(l->dist2(p),-r->dist2(p));
    }

    Difference::Difference(Implicit *l, Implicit *r) : ImplicitNode(l, r) {}
} // Engine