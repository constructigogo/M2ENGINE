//
// Created by constantin on 13/12/23.
//

#include "PBDContainer.h"
#include "../Implicits/ImplicitAll.h"

namespace Engine {
    void PBDContainer::reset() {
        _data.reset();
    }

    void PBDContainer::generateParticules(float initialVelocity) {
        std::vector<glm::vec3> newParts;

        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY / 2; ++y) {
                for (int z = 0; z < _sizeZ; ++z) {
                    glm::vec3 gridPos(x + 0.5, y + 0.5, z + 0.5);
                    glm::vec3 partPos = gridPos * _cellSize + _origin;
                    newParts.push_back(partPos);
                }
            }
        }

        _data.addParticles(newParts, initialVelocity);
    }

    void PBDContainer::findNeighbors(const std::vector<glm::vec3> &allPos, std::vector<std::vector<int>> &nbors) {
        KDTree<glm::vec3, 3> kdTree;
        kdTree.build(allPos);

#pragma omp parallel for
        for (int i = 0; i < allPos.size(); ++i) {
            std::vector<int> cand = kdTree.radiusSearch(allPos[i], _data.radius * 2.0f);
            for (int candI: cand) {
                if (candI == i) continue;
                nbors[i].push_back(candI);
            }
        }

        kdTree.clear();
    }

    void PBDContainer::step(float deltaTime, int substep) {
        float subDt = deltaTime / (float) substep;

        std::vector<std::vector<int>> nbors(_data.Positions.size());

        findNeighbors(_data.Positions, nbors);

        std::vector<glm::vec3> newPos(_data.Positions.size());

        float boundaryFriction = pow(1.0f - _friction, 1 / (float)(numConstraintSubStep));
        float partFriction = pow(1.0f - _friction, 1 / (float)(numConstraintSubStep));

        for (int step = 0; step < substep; ++step) {
#pragma omp parallel for
            for (int idx = 0; idx < _data.Positions.size(); ++idx) {
                _data.Velocities[idx] += glm::vec3(0.f, -9.81f * subDt, 0.f);
                newPos[idx] = _data.Positions[idx] + _data.Velocities[idx] * subDt;
            }

            for (int i = 0; i < numConstraintSubStep; ++i) {
                solveBoundary(newPos, boundaryFriction);
                solveParticle(newPos, nbors, partFriction);
            }

#pragma omp parallel for
            for (int i = 0; i < _data.Positions.size(); ++i) {
                _data.Velocities[i] = (newPos[i] - _data.Positions[i]) / subDt;
                _data.Positions[i] = newPos[i];
            }
        }
    }

    void PBDContainer::solveParticle(std::vector<glm::vec3> &pos, const std::vector<std::vector<int>> &nbors,
                                     float friction) {
        std::vector<std::vector<int>> collision(pos.size());


        for (int idx = 0; idx < pos.size(); ++idx) {
            for (auto &nbor: nbors[idx]) {
                if (nbor < idx)continue;
                glm::vec3 dir = pos[idx] - pos[nbor];
                float dist = glm::length(dir) - _data.radius;
                if (dist < 0) {
                    collision[idx].push_back(nbor);
                    collision[nbor].push_back(idx);
                    float str = -0.25f * dist;
                    dir = str * glm::normalize(dir);
                    pos[idx] += dir;
                    pos[nbor] -= dir;
                }
            }
        }

#pragma omp parallel for
        for (int idx = 0; idx < pos.size(); ++idx) {
            glm::vec3 &current = pos[idx];
            for (int colIdx: collision[idx]) {
                glm::vec3 norm = glm::normalize(current - pos[colIdx]);
                glm::vec3 deltaPos = current - _data.Positions[idx];
                glm::vec3 deltaNorm = glm::dot(deltaPos, norm) * norm;
                glm::vec3 deltaTangent = deltaPos - deltaNorm;
                current = _data.Positions[idx] + deltaNorm + friction * deltaTangent;
            }
        }
    }


    void PBDContainer::solveBoundary(std::vector<glm::vec3> &pos, float friction) {
        glm::vec3 center = _origin;
        Sphere sp(center, 9);

#pragma omp parallel for
        for (int idx = 0; idx < data().size(); idx++) {
            auto &p = pos[idx];
            float dist = sp.dist(p)+_data.radius;
            glm::vec3 N;
            if (dist > 0) {
                glm::vec3 dir = glm::normalize(p - center);
                N= dir;
                p -= 0.5f * dist * dir;
                glm::vec3 delta = p - _data.Positions[idx];
                glm::vec3 normalDelta = dot(delta, N) * N;
                glm::vec3 tangentialDelta = delta - normalDelta;

                p = _data.Positions[idx] + normalDelta + friction * tangentialDelta;
            }
        }
    }

    void PBDContainer::update(float deltaTime, int substep) {
        step(deltaTime, substep);
    }

} // Engine