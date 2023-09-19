//
// Created by constantin on 06/09/23.
//

#include "Ray.h"

namespace Engine {
    Ray::Ray(const glm::vec3 &origin, const glm::vec3 &direction) : origin(origin), direction(direction) {}
} // Engine