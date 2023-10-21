//
// Created by constantin on 19/10/23.
//

#pragma once
#include "Implicit.h"
namespace Engine {

    class CappedConeExact : public ImplicitTransform{
    public:
        CappedConeExact() = delete;

        CappedConeExact(const glm::vec3 &pos, const glm::vec3 &rot, float height, float r1, float r2);

        float dist(const glm::vec3 &_p) const override;

        float dist2(const glm::vec3 &p) const override;

    protected:
        float height;
        float r1;
        float r2;
    private:
    };

} // Engine
