//
// Created by constantin on 18/10/23.
//

#pragma once

#include "Implicit.h"

namespace Engine {

    class BoxFrame : public Implicit {
    public:
        BoxFrame() = delete;

        explicit BoxFrame(const glm::vec3& pos,const glm::vec3 &size, float _frameSize);

        float dist(const glm::vec3 &_p) const override;

        float dist2(const glm::vec3 &_p) const override;

        ~BoxFrame() override = default;

    protected:
        glm::vec3 position;
        glm::vec3 boxSize;
        float frameSize;
    private:
    };

} // Engine
