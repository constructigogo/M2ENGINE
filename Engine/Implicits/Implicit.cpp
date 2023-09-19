//
// Created by constantin on 06/09/23.
//

#include "Implicit.h"

namespace Engine {
    bool Implicit::isInside(glm::vec3 p) {
        return dist2(p)<0;
    }
} // Engine