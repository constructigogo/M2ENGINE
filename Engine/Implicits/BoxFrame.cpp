//
// Created by constantin on 18/10/23.
//

#include "BoxFrame.h"
#include "glm/glm.hpp"

namespace Engine {
    BoxFrame::BoxFrame(const glm::vec3& pos, const glm::vec3 &size, float frameSize) : position(pos), boxSize(size), frameSize(frameSize) {}

    float BoxFrame::dist(const glm::vec3 &_p) const {
        glm::vec3 p = abs(_p-position) - boxSize;
        glm::vec3 q = abs(p + frameSize) - frameSize;

        return glm::min(glm::min(
                           length(glm::max(glm::vec3(p.x, q.y, q.z), 0.0f)) + glm::min(glm::max(p.x, glm::max(q.y, q.z)), 0.0f),
                           length(glm::max(glm::vec3(q.x, p.y, q.z), 0.0f)) + glm::min(glm::max(q.x, glm::max(p.y, q.z)), 0.0f)),
                   length(glm::max(glm::vec3(q.x, q.y, p.z), 0.0f)) + glm::min(glm::max(q.x, glm::max(q.y, p.z)), 0.0f));
    }

    float BoxFrame::dist2(const glm::vec3 &_p) const {
        float val = dist(_p);
        return val*val;
    }

} // Engine