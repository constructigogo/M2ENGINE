//
// Created by constantin on 20/12/23.
//

#pragma once

#include "Grid.h"
#include "glm/glm.hpp"

namespace Engine {

    class ScalarField : public Grid<float> {
    public:
        ScalarField(int sizeX, int sizeY);

        glm::vec3& gradient(int x,int y);


        glm::vec3 gradientAt(int x, int y) const;
        ScalarField buildGradientNorm();
        ScalarField buildLaplacian();

        void normalize();
        void clamp(float min,float max);

        void smooth(int size);
        void blur(int size);

    protected:
        int _cachedMin;
        int _cachedMax;
    private:
    };

} // Engine
