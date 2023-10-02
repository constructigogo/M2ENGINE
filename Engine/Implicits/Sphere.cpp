//
// Created by constantin on 13/09/23.
//

#include "Sphere.h"

namespace Engine {
    Sphere::Sphere(glm::vec3 _o, float _r) : o(_o), r(_r) {

    }

    float Sphere::dist(const glm::vec3 &p) const {
        auto val =glm::distance(o,p)-r;
        return val ;
    }

    float Sphere::dist2(const glm::vec3 &p) const {
        glm::vec3 t(o-p);
        return glm::dot(t,t);
    }

    Sphere::~Sphere() {};
} // Engine