//
// Created by constantin on 20/12/23.
//

#pragma once

#include "ScalarField.h"

namespace Engine {

    class HeightField : public ScalarField {
    public:
        HeightField(int sizeX, int sizeY, float scale=1.0) : ScalarField(sizeX, sizeY), scale(scale) {}

        float heightAt(int x, int y) const;

        float slopeAt(int x, int y);

        float averageSlopeAt(int x, int y);

        glm::vec3 normalAt(int x, int y) const;

        float getScale() const;

    protected:
        float scale;
    private:
    };


} // Engine
