//
// Created by constantin on 13/09/23.
//

#pragma once
#include "ImplicitNode.h"
namespace Engine {

    class Union : public ImplicitNode{
    public:
        explicit Union(Implicit *l, Implicit *r);

        float dist(const glm::vec3 &p) const override;
        float dist2(const glm::vec3 &p) const override;
        ~Union() override =default;
    protected:

    private:
    };

} // Engine
