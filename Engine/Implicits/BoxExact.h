//
// Created by constantin on 18/10/23.
//

#pragma once

#include "Implicit.h"

namespace Engine {

    class BoxExact : public ImplicitTransform {
    public:
        BoxExact() = delete;

        BoxExact(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &boxDim);

        float dist(const glm::vec3 &_p) const override;

        float dist2(const glm::vec3 &_p) const override;

        ~BoxExact() override = default;

    protected:
        glm::vec3 BoxDim;
    private:
    };

} // Engine
