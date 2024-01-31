//
// Created by constantin on 20/12/23.
//

#include "HeightField.h"
#include "Log.h"

namespace Engine {
    float HeightField::heightAt(int x, int y) const {
        return at(x, y) * scale;
    }

    float HeightField::getScale() const {
        return scale;
    }

    glm::vec3 HeightField::normalAt(int x, int y) const {
        glm::vec3 startX;
        glm::vec3 startY;
        glm::vec3 endX;
        glm::vec3 endY;

        glm::vec3 resX;
        glm::vec3 resY;
        if (x == 0) {
            startX = glm::vec3(x, heightAt(x, y), y);
            endX = glm::vec3(x + 1, heightAt(x + 1, y), y);
        } else if (x == _sizeX - 1) {
            startX = glm::vec3(x - 1, heightAt(x - 1, y), y);
            endX = glm::vec3(x, heightAt(x, y), y);
        } else {
            startX = glm::vec3(x - 1, heightAt(x - 1, y), y);
            endX = glm::vec3(x + 1, heightAt(x + 1, y), y);
        }

        if (y == 0) {
            startY = glm::vec3(x, heightAt(x, y), y);
            endY = glm::vec3(x, heightAt(x, y + 1), y + 1);
        } else if (y == _sizeY - 1) {
            startY = glm::vec3(x, heightAt(x, y - 1), y - 1);
            endY = glm::vec3(x, heightAt(x, y), y);
        } else {
            startY = glm::vec3(x, heightAt(x, y - 1), y - 1);
            endY = glm::vec3(x, heightAt(x, y + 1), y + 1);
        }

        glm::vec3 X = glm::normalize(endX - startX);
        glm::vec3 Y = glm::normalize(endY - startY);
        glm::vec3 res = glm::normalize(glm::cross(X, -Y));
        //res = glm::vec3(res.x,res.z,res.y);

        return res;
    }

    HeightField HeightField::thermalErosion(float angle, float dt, int steps) {
        ENGINE_DEBUG("Begin thermal erosion");
        float tAngle = std::tan(angle);
        HeightField current(*this);
        HeightField next(*this);
        for (int step = 0; step < steps; ++step) {
            ENGINE_TRACE("Thermal Erosion step {}/{}", step + 1, steps);
            ScalarField laplacian = current.buildLaplacian();
            for (int x = 0; x < _sizeX; ++x) {
                for (int y = 0; y < _sizeY; ++y) {
                    float h = current.heightAt(x, y);
                    float maxSlope = 0;
                    int bx, by;
                    for (int ox = -1; ox <= 1; ++ox) {
                        for (int oy = 0; oy <= 1; ++oy) {
                            if (ox == 0 && oy == 0) continue;
                            int ex = x + ox;
                            int ey = y + oy;

                            if (!isInside(ex, ey)) continue;

                            float localH = current.heightAt(ex, ey);
                            float slope = h - localH;
                            if (slope > tAngle && slope > maxSlope) {
                                maxSlope = slope;
                                bx = ox;
                                by = oy;
                            }
                        }
                    }
                    if (maxSlope != 0.0) {
                        int ex = x + bx;
                        int ey = y + by;
                        float volume = dt * (maxSlope - tAngle);
                        next.at(x, y) -= volume - dt * laplacian.at(x, y);
                        next.at(ex, ey) += volume;
                    }
                }
            }
            current = std::move(next);
            next = HeightField(current);
        }
        ENGINE_DEBUG("Finished thermal erosion");

        return next;
    }

    ScalarField HeightField::buildStreamArea() {
        ScalarField gradient = buildGradient();
        ScalarField neight(_sizeX, _sizeY);
        ScalarField StreamArea(_sizeX, _sizeY,1.0f);

        float totalSlope = 0.0f;

//#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                float actual = heightAt(x, y);
                for (int ox = -1; ox <= 1; ++ox) {
                    for (int oy = -1; oy <= 1; ++oy) {
                        if (ox == 0 && oy == 0) continue;
                        int ex = x + ox;
                        int ey = y + oy;
                        if (!isInside(ex, ey)) continue;

                        float nActual = heightAt(ex, ey);

                        if (nActual < actual) {
                            float nSlope = gradient.at(x, y) + actual - nActual;
                            totalSlope += actual - nActual;
                            gradient.at(x, y) = nSlope;
                            neight.at(x, y)++;
                        }
                    }
                }
            }
        }

#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                StreamArea.at(x,y)=1.0f;
            }
        }
#pragma omp parallel for
        for (int x = 0; x < _sizeX; ++x) {
            for (int y = 0; y < _sizeY; ++y) {
                StreamArea.at(x,y)=StreamArea.at(x,y) * gradient.at(x,y) / std::max(1.0f, std::abs(totalSlope));
            }
        }

        return StreamArea;
    }

    HeightField HeightField::hillSlope(float k, float dt, int steps) {
        HeightField res(*this);

        for (int step = 0; step < steps; ++step) {
            ENGINE_TRACE("Hill Slope step {}/{}", step + 1, steps);
            ScalarField laplacian = res.buildLaplacian();
#pragma omp parallel for
            for (int x = 0; x < _sizeX; ++x) {
                for (int y = 0; y < _sizeY; ++y) {
                    float volume = k * dt * laplacian.at(x, y);
                    res.at(x, y) += volume;
                }
            }
        }
        return res;
    }

    HeightField HeightField::streamPower(float u, float k, float dt, int steps) {
        HeightField res(*this);

        for (int step = 0; step < steps; ++step) {
            ENGINE_TRACE("Stream power step {}/{}", step + 1, steps);
            //ScalarField laplacian = res.buildLaplacian();
            ScalarField streamArea = buildStreamArea();
            ScalarField gradient = buildGradient();
#pragma omp parallel for
            for (int x = 0; x < _sizeX; ++x) {
                for (int y = 0; y < _sizeY; ++y) {
                    float volume = dt * k * sqrtf(streamArea.at(x, y) * gradient.at(x, y));
                    res.at(x, y) -= volume;
                }
            }
        }
        return res;
    }


} // Engine