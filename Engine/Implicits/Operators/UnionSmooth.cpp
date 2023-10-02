//
// Created by constantin on 28/09/23.
//

#include "UnionSmooth.h"

namespace Engine {

    float UnionSmooth::dist(const glm::vec3 &p) const {
        //const double smoothUnionValue = exp(-m_factor * l->dist(p)) + exp(-m_factor * r->dist(p));
        const float dl = l->dist(p);
        const float dr = r->dist(p);
        const float h = std::max(m_factor - std::abs(dl - dr), 0.f) / m_factor;
        return std::min(dl, dr) - ((1.f/6.f) * m_factor * h * h * h);
    }

    float UnionSmooth::dist2(const glm::vec3 &p) const {
        float dl = l->dist2(p);
        float dr = r->dist2(p);
        const float h = std::max(m_factor - std::abs(dl - dr), 0.f) / m_factor;
        return std::min(dl, dr) - ((1.f/6.f) * m_factor * h * h * h);
    }

    UnionSmooth::UnionSmooth(Implicit *l, Implicit *r, float mFactor) : ImplicitNode(l, r), m_factor(mFactor) {}


} // Engine