//
// Created by constantin on 13/09/23.
//

#include "Union.h"

namespace Engine {
    float Union::dist(const glm::vec3 &p) const {
        //return std::sqrt(std::min(l->dist2(p),r->dist2(p)));
        return std::min(l->dist(p),r->dist(p));
    }

    float Union::dist2(const glm::vec3 &p) const {
        return std::min(l->dist2(p),r->dist2(p));
    }

    Union::Union(Implicit *l, Implicit *r) : ImplicitNode(l, r) {

    }
} // Engine