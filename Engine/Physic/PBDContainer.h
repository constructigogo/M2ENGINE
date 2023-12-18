//
// Created by constantin on 13/12/23.
//

#pragma once

#include "ParticleData.h"
#include "../Core/Box.h"
#include "../Core/KDTree.h"

namespace Engine {

    class PBDContainer {
    public:
        PBDContainer(const glm::vec3 &origin, float cellSize, int sizeX, int sizeY, int sizeZ, int numConstraintSubStep)
                : _origin(origin), _cellSize(cellSize), _sizeX(sizeX), _sizeY(sizeY), _sizeZ(sizeZ),
                  numConstraintSubStep(numConstraintSubStep),
                  bbox(glm::vec3(0.0), glm::vec3(sizeX, sizeY, sizeZ) * cellSize) {

            _data.mass = .5f;
            _data.radius = 0.2f;
            _friction=0.5f;
            generateParticules(-1.f);
        }

        const std::vector<glm::vec3> &data() const {
            return _data.Positions;
        }

        void setMass(float m) {
            _data.mass = m;
        }

        float getMass() const { return _data.mass; }

        int getNumConstraintSubStep() const {
            return numConstraintSubStep;
        }

        void setNumConstraintSubStep(int n) {
            PBDContainer::numConstraintSubStep = n;
        }

        const glm::vec3 &getOrigin() const {
            return _origin;
        }

        void setOrigin(const glm::vec3 &origin) {
            _origin = origin;
        }

        void setParticleSize(float s) {
            _data.radius = s;
        }

        float getParticleSize() const { return _data.radius; }


        float getFriction() const {
            return _friction;
        }

        void setFriction(float friction) {
            _friction = friction;
        }


        void update(float deltaTime, int substep);

        void reset();

    protected:
        void generateParticules(float initialVelocity);

        void findNeighbors(const std::vector<glm::vec3> &allPos, std::vector<std::vector<int>> &nbors);

        void step(float deltaTime, int substep);

        void solveParticle(std::vector<glm::vec3> &pos, const std::vector<std::vector<int>> &nbors, float friction);

        void solveBoundary(std::vector<glm::vec3> &pos, float friction);


        ParticleData _data;
        glm::vec3 _origin;
        float _cellSize;
        float _friction;
        int _sizeX;
        int _sizeY;
        int _sizeZ;
        int numConstraintSubStep;
        Box bbox;

    private:
    };

} // Engine
