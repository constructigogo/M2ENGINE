//
// Created by constantin on 28/09/23.
//

#pragma once
#include "ImplicitNode.h"
namespace Engine {

    class DifferenceSmooth : public ImplicitNode{
    public:
        DifferenceSmooth(Implicit *l, Implicit *r, float mFactor=0.4);

        float dist(const glm::vec3 &p) const override;

        float dist2(const glm::vec3 &p) const override;

    protected:
        float m_factor;
    private:
    };

} // Engine
