//
// Created by Constantin on 22/07/2023.
//

#include "Object.h"

namespace Engine {
    Object::~Object() {
        for (auto comp : m_components) {
            comp.second->cleanup() ;
        }
        for (auto comp : m_components) {
            delete comp.second ;
        }
    }

}
