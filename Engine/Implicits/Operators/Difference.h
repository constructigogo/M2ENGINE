//
// Created by constantin on 28/09/23.
//

#pragma once
#include "ImplicitNode.h"
namespace Engine {

    class Difference : public ImplicitNode{
    public:
        explicit Difference(Implicit *l, Implicit *r);

        float dist(const glm::vec3 &p) const override;
        float dist2(const glm::vec3 &p) const override;

    protected:

    private:
    };

} // Engine
