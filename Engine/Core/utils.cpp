//
// Created by Constantin on 20/07/2023.
//

#include <string>
#include <fstream>
#include <iostream>
#include "utils.h"
#include "glm/gtc/quaternion.hpp"

namespace Engine::Utils {
    glm::mat3 QuaternionToMat(glm::quat quat) {
        glm::quat glQuat(quat.w, quat.x, quat.y, quat.z);
        return mat3_cast(glQuat);
    }

    float ndot(glm::vec2 a, glm::vec2 b) { return a.x * b.x - a.y * b.y; }


    float dot2( glm::vec2 v ) { return dot(v,v); }
    float dot2( glm::vec3 v ) { return dot(v,v); }

    int factorial(int n){
        return (n<=1)? 1: factorial(n-1)*n;
    }

}



