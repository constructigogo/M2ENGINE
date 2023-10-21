//
// Created by constantin on 21/10/23.
//

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Engine {
    class Bezier {
    public:
        explicit Bezier(const std::vector<glm::vec3> &controlPoints);
        explicit Bezier(const std::vector<std::vector<glm::vec3>> &controlPoints);

        glm::vec3 eval(float u, float v = 0) const;


    protected:
        int n;
        int m;

        float bernstein(int n, int i, float t) const;

        std::vector<glm::vec3> controlPoints;
    private:
    };

}