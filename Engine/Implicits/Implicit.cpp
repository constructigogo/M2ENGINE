//
// Created by constantin on 06/09/23.
//

#include "Implicit.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
namespace Engine {
    bool Implicit::isInside(glm::vec3 p) {
        return dist2(p)<0;
    }

    ImplicitTransform::ImplicitTransform(const glm::vec3 &pos, const glm::vec3 &rot) : pos(-pos), rot(rot) {
        //this->pos.x = -pos.x;
        glm::mat4 model_matrix(1.0f);
        auto translate = glm::translate(model_matrix, this->pos);
        auto rotate = glm::mat4_cast(glm::quat(rot));
        glm::mat4 _mtx = rotate*translate;
        this->mtx = _mtx;
    }

    glm::vec3 ImplicitTransform::transPoint(const glm::vec3 &_p) const {
        return mtx * glm::vec4(_p,1.0);
    }
} // Engine