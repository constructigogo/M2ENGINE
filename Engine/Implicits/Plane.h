//
// Created by constantin on 18/10/23.
//

#pragma once
#include "Implicit.h"
namespace Engine {

    class Plane : public ImplicitTransform{
    public:
        Plane() = delete;

        Plane(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &normal);

        float dist(const glm::vec3 &p) const override;

        float dist2(const glm::vec3 &p) const override;

    protected:
        glm::vec3 normal;
    private:
    };

} // Engine
