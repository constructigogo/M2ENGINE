//
// Created by constantin on 20/12/23.
//

#pragma once

#include "ScalarField.h"

namespace Engine {

    class HeightField : public ScalarField {
    public:
        HeightField(int sizeX, int sizeY, float scale=1.0) : ScalarField(sizeX, sizeY), scale(scale) {}

        HeightField thermalErosion(float angle, float dt, int steps);
        HeightField hillSlope(float k, float dt, int steps);
        HeightField streamPower(float u, float k, float dt, int steps);
        ScalarField buildStreamArea();


        float heightAt(int x, int y) const;

        float slopeAt(int x, int y);

        glm::vec3 normalAt(int x, int y) const;

        float getScale() const;

    protected:
        float scale;
    private:
    };


} // Engine
