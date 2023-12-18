//
// Created by constantin on 13/12/23.
//

#include "ParticleData.h"

namespace Engine {
    void ParticleData::reset() {
        Positions.resize(0);
        Velocities.resize(0);
    }

    void ParticleData::addParticles(const std::vector<glm::vec3>& particules, float initialVelocity) {
        Positions.insert(Positions.end(),particules.begin(),particules.end());
        Velocities.resize(Positions.size(),glm::vec3(0,-initialVelocity,0));
    }
} // Engine