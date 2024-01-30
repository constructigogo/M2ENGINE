//
// Created by constantin on 20/12/23.
//

#include "ScalarField.h"
#include "cmath"
#include "Log.h"

namespace Engine {
    ScalarField::ScalarField(int sizeX, int sizeY) : Grid(sizeX, sizeY, 0.f) {
        _cachedMin = 0;
        _cachedMax = 0;
    }

    glm::vec3 ScalarField::gradientAt(int x, int y) const {

        float height = at(x, y);

        float dx = 0;
        float dy = 0;

        int nx = 0;
        int ny = 0;

        if (x > 0) {
            dx -= at(x - 1, y) - height;
            nx++;
        }
        if (x < _sizeX - 1) {
            dx += at(x + 1, y) - height;
            nx++;
        }
        if (y > 0) {
            dy -= at(x, y - 1) - height;
            ny++;
        }
        if (y < _sizeY - 1) {
            dy += at(x, y + 1) - height;
            ny++;
        }
        dx = dx / nx;
        dy = dy / ny;
        glm::vec2 res = glm::vec2(dx, dy);
        return glm::vec3(glm::normalize(res), glm::length(res));
    }

    ScalarField ScalarField::buildGradient() const {
        ScalarField res(_sizeX, _sizeY);
#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                res.at(x, y) = gradientAt(x, y).z;
            }
        }
        return res;
    }

    ScalarField ScalarField::buildGradientNorm() const {
        ScalarField res = buildGradient();
        res.normalize();
        return res;
    }

    void ScalarField::normalize() {
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                float &val = at(x, y);
                min = std::min(val, min);
                max = std::max(val, max);
            }
        }

        auto reduce = ([min, max](float v) -> float {
            return (v - min) / (max - min);
        });

#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                float &val = at(x, y);
                val = reduce(val);
            }
        }
    }

    void ScalarField::clamp(float min, float max) {
#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                float &val = at(x, y);
                val = std::clamp(val, min, max);
            }
        }
    }

    float ScalarField::laplacianAt(int x, int y) const {
        float center = at(x, y);

        float left, right, up, down;


        if (x > 0) {
            left = at(x - 1, y);
        } else {
            left = center;
        }

        if (x < _sizeX - 1) {
            right = at(x + 1, y);
        } else {
            right = center;
        }

        if (y > 0) {
            down = at(x, y - 1);
        } else {
            down = center;
        }

        if (y < _sizeY - 1) {
            up = at(x, y + 1);
        } else {
            up = center;
        }

        float dx = 1.0f;
        float dy = 1.0f;
        return (left + right + up + down - 4 * center) / (dx * dx);
    }

    ScalarField ScalarField::buildLaplacian() const {
        ScalarField res(_sizeX, _sizeY);
#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                float val = laplacianAt(x, y);
                res.at(x, y) = val;
            }
        }
        return res;
    }

    ScalarField ScalarField::buildLaplacianNorm() const {
        ScalarField res = buildLaplacian();
        res.normalize();
        return res;
    }


} // Engine