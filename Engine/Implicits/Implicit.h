//
// Created by constantin on 06/09/23.
//

#pragma once


#include "glm/glm.hpp"
#include "glm/common.hpp"
#include "../Core/Ray.h"
#include <cmath>

namespace Engine {

    class Implicit {
    public:
        Implicit()= default;

        virtual float dist(const glm::vec3 &p) const =0;
        virtual float dist2(const glm::vec3 &p) const =0;
        virtual bool isInside(glm::vec3 p);
        virtual bool intersect(const Ray& ray, RayHit& hit){return false;}

        virtual ~Implicit()= default;
    protected:

    private:
    };

} // Engine
