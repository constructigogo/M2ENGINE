//
// Created by constantin on 21/10/23.
//

#include "Bezier.h"
#include "../Core/utils.h"
#include "glm/vec3.hpp"

namespace Engine {
    glm::vec3 Bezier::eval(float u, float v) const {
        if (n == 2 && m == 1) {
            return glm::mix(controlPoints[0], controlPoints[1], u);
        }

        if (m == 1) {
            std::vector<glm::vec3> subU;
            subU.reserve(n - 1);
            for (int i = 0; i < n - 1; ++i) {
                subU.push_back(glm::mix(controlPoints[i], controlPoints[i + 1], u));
            }
            Bezier subBU(subU);
            return subBU.eval(u);
        }else {
            std::vector<glm::vec3> subV;
            subV.reserve(m - 1);
            for (int j = 0; j < m; ++j) {
                std::vector<glm::vec3> subU;
                subU.reserve(n - 1);
                for (int i = 0; i < n - 1; ++i) {
                    subU.push_back(glm::mix(controlPoints[j*m+ i], controlPoints[j*m+i + 1], u));
                }
                Bezier subBU(subU);
                subV.push_back(subBU.eval(u));
            }
            Bezier subBV(subV);
            glm::vec3 uv = subBV.eval(v);
            return uv;
        }


    }

    float Bezier::bernstein(int n, int i, float t) const {
        return float(float(Utils::factorial(n))) / float(Utils::factorial(n) * Utils::factorial(n - i)) *
               std::pow(t, i) * std::pow(1.0 - t, n - i);
    }

    Bezier::Bezier(const std::vector<glm::vec3> &controlPoints) : controlPoints(controlPoints) {
        n = this->controlPoints.size();
        m = 1;
    }

    Bezier::Bezier(const std::vector<std::vector<glm::vec3>> &controlPoints) {
        n = controlPoints.size();
        m = controlPoints[0].size();
        this->controlPoints.reserve(n * m);
        for (auto &curve: controlPoints) {
            this->controlPoints.insert(this->controlPoints.end(), curve.begin(), curve.end());
        }
    }
}