//
// Created by constantin on 18/10/23.
//

#include "Rombus.h"
#include "../Core/utils.h"
namespace Engine {
    Rombus::Rombus(const glm::vec3 &pos, const glm::vec3 &rot, float la, float lb, float h, float ra)
            : ImplicitTransform(pos, rot), la(la), lb(lb), h(h), ra(ra) {}

    float Rombus::dist(const glm::vec3 &_p) const {
        glm::vec3 p = abs(transPoint(_p));
        glm::vec2 b = glm::vec2(la,lb);
        float f = glm::clamp((Utils::ndot(b,b-2.0f*glm::vec2(p.x,p.z)))/dot(b,b), -1.0f, 1.0f );
        glm::vec2 q = glm::vec2(length(glm::vec2(p.x,p.z)-0.5f*b*glm::vec2(1.0-f,1.0+f))*glm::sign(p.x*b.y+p.z*b.x-b.x*b.y)-ra, p.y-h);
        return glm::min(glm::max(q.x,q.y),0.0f) + length(glm::max(q,0.0f));
    }

    float Rombus::dist2(const glm::vec3 &p) const {
        return dist(p);
    }
} // Engine