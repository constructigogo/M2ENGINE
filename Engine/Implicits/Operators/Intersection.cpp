//
// Created by constantin on 28/09/23.
//

#include "Intersection.h"

namespace Engine {
    Intersection::Intersection(Implicit *l, Implicit *r) : ImplicitNode(l, r) {}

    float Intersection::dist(const glm::vec3 &p) const {
        return std::max(l->dist(p),r->dist(p));
    }

    float Intersection::dist2(const glm::vec3 &p) const {
        return std::max(l->dist2(p),r->dist2(p));
    }
} // Engine