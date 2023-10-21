//
// Created by constantin on 19/10/23.
//

#include "CappedConeExact.h"
#include "../Core/utils.h"
namespace Engine {
    CappedConeExact::CappedConeExact(const glm::vec3 &pos, const glm::vec3 &rot, float height, float r1, float r2)
            : ImplicitTransform(pos, rot), height(height), r1(r1), r2(r2) {}

    float CappedConeExact::dist(const glm::vec3 &_p) const {
        glm::vec3 p = transPoint(_p);
        float h= height;
        glm::vec2 q = glm::vec2( length(glm::vec2(p.x,p.z)), p.y );
        glm::vec2 k1 = glm::vec2(r2,h);
        glm::vec2 k2 = glm::vec2(r2-r1,2.0*h);
        glm::vec2 ca = glm::vec2(q.x-glm::min(q.x,(q.y<0.0)?r1:r2), abs(q.y)-h);
        glm::vec2 cb = q - k1 + k2*glm::clamp( dot(k1-q,k2)/Utils::dot2(k2), 0.0f, 1.0f );
        float s = (cb.x<0.0 && ca.y<0.0) ? -1.0 : 1.0;
        return s*sqrtf( glm::min(Utils::dot2(ca),Utils::dot2(cb)));
    }

    float CappedConeExact::dist2(const glm::vec3 &p) const {
        float val=dist(p);
        return val*val;
    }
} // Engine