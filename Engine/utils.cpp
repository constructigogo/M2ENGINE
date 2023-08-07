//
// Created by Constantin on 20/07/2023.
//

#include <string>
#include <fstream>
#include <iostream>
#include "utils.h"
#include "glm/gtc/quaternion.hpp"

namespace Engine::Utils{
    glm::mat3 bxQuaternionToMat(bx::Quaternion quat){
        glm::quat glQuat(quat.w,quat.x,quat.y,quat.z);
        return mat3_cast(glQuat);
    }
}



