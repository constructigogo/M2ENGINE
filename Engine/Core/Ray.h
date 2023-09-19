//
// Created by constantin on 06/09/23.
//

#pragma once
#include "glm/vec3.hpp"
#include "../Objects/Object.h"

namespace Engine {

    struct RayHit {
        Object * hitRef;
        glm::vec3 hitPosition;
        glm::vec3 hitNormal;
    };

    class Ray {
    public:
        glm::vec3 origin;
        glm::vec3 direction;
        Ray() = default;
        Ray(const glm::vec3 &origin, const glm::vec3 &direction);

    protected:

    private:
    };

} // Engine
