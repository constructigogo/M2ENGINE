//
// Created by constantin on 18/10/23.
//

#include "Plane.h"

namespace Engine {
    Plane::Plane(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &normal) : ImplicitTransform(pos, rot),
                                                                                        normal(glm::normalize(normal)) {}

    float Plane::dist(const glm::vec3 &p) const {
        return dot(transPoint(p),normal);
    }

    float Plane::dist2(const glm::vec3 &p) const {
        float val = dist(p);
        return val*val;
    }
} // Engine