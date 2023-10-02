//
// Created by constantin on 13/09/23.
//

#pragma once

#include "Implicit.h"

namespace Engine {

    class Sphere : public Implicit{
    public:
        explicit Sphere(glm::vec3 _o, float _r);

        float dist(const glm::vec3 &p) const override;

        float dist2(const glm::vec3 &p) const override;

        ~Sphere() override;
    protected:
        glm::vec3 o;
        float r;
    private:
    };



} // Engine
