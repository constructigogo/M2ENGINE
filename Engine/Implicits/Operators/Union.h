//
// Created by constantin on 13/09/23.
//

#pragma once
#include "ImplicitNode.h"
namespace Engine {

    class Union : public ImplicitNode{
    public:
        Union(Implicit *l, Implicit *r);

        float dist(glm::vec3 p) override;

        float dist2(glm::vec3 p) override;
        ~Union() override =default;
    protected:

    private:
    };

} // Engine
