//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

#include <cstring>
#include <cstdio>
#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "bx/math.h"

namespace Engine::Utils {
    glm::mat3 QuaternionToMat(glm::quat quat);
    float ndot(glm::vec2 a, glm::vec2 b);

    float dot2( glm::vec2 v );
    float dot2( glm::vec3 v );
    int factorial(int n);
}

#endif //ENGINE_UTILS_H
