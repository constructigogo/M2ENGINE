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
        glm::mat3 bxQuaternionToMat(bx::Quaternion);
}

#endif //ENGINE_UTILS_H
