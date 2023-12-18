//
// Created by constantin on 13/12/23.
//

#pragma once
#include <vector>
#include "glm/glm.hpp"

namespace Engine {

    struct ParticleData {
        float radius;
        float mass;
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec3> Velocities;

        void addParticles(const std::vector<glm::vec3>& particules,float initialVelocity);
        void addParticle(glm::vec3 particule,float initialVelocity);

        void reset();
    };

} // Engine
