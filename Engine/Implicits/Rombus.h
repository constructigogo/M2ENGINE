//
// Created by constantin on 18/10/23.
//

#pragma once
#include "Implicit.h"
namespace Engine {

    class Rombus : public ImplicitTransform{
    public:
        Rombus() = delete;

        Rombus(const glm::vec3 &pos, const glm::vec3 &rot, float la, float lb, float h, float ra);

        float dist(const glm::vec3 &_p) const override;

        float dist2(const glm::vec3 &p) const override;

    protected:
        float la;
        float lb;
        float h;
        float ra;
    private:
    };

} // Engine
