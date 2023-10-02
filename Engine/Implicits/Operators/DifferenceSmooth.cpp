//
// Created by constantin on 28/09/23.
//

#include "DifferenceSmooth.h"

namespace Engine {

    float DifferenceSmooth::dist(const glm::vec3 &p) const {
        const float dl = l->dist(p);
        const float dr = -r->dist(p);
        const float h = std::max(m_factor - std::abs(dl - dr), 0.f) / m_factor;
        return (std::max(dl, dr) + ((1.f/6.f) * m_factor * h * h * h));
    }

    float DifferenceSmooth::dist2(const glm::vec3 &p) const {
        const float dl = l->dist2(p);
        const float dr = r->dist2(p);
        const float h = std::max(m_factor - std::abs(dl - dr), 0.f) / m_factor;
        return (std::max(dl, -dr) - ((1.f/6.f) * m_factor * h * h * h));
    }

    DifferenceSmooth::DifferenceSmooth(Implicit *l, Implicit *r, float mFactor) : ImplicitNode(l, r),
                                                                                  m_factor(mFactor) {}
} // Engine