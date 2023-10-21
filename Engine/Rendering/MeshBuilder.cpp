//
// Created by constantin on 21/10/23.
//

#include "MeshBuilder.h"

namespace Engine {
    std::shared_ptr<BaseMesh> MeshBuilder::Polygonize(const Bezier &bezier, int n) {
        std::vector<glm::vec3> poses;
        poses.resize(n * n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                float u = (float) i / float(n - 1);
                float v = (float) j / float(n - 1);
                glm::vec3 pos = bezier.eval(u, v);
                poses[(i * n) + j] = pos;
            }
        }
        std::vector<BaseMesh::vertexData> data;
        std::vector<uint32_t> indices;
        for (auto &pos: poses) {
            data.emplace_back(
                    pos,
                    glm::vec3(0.0),
                    glm::vec3(0.0),
                    glm::vec2(0.0),
                    0
            );
        }
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - 1; ++j) {
                indices.push_back(i * n + j);
                indices.push_back((i) * n + j + 1);
                indices.push_back((i + 1) * n + j + 1);

                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j + 1);
                indices.push_back((i + 1) * n + j);
            }
        }
        BaseMesh::SubMesh sub;
        sub.init(data, indices, true);
        auto res = std::make_shared<BaseMesh>();
        res->addSubMesh(sub);
        return res;
    }

    std::shared_ptr<BaseMesh> MeshBuilder::PolygonizeRevolution(const Bezier &axis, const Bezier &bezier, int n) {
        std::vector<glm::vec3> poses;
        poses.resize(2*(n-1)*(n-1));
        for (int i = 0; i < n; ++i) {
            float u = (float) i / float(n - 1);
            glm::vec3 q = axis.eval(u);
            for (int j = 0; j < n; ++j) {
                float theta = -2.0 * j * M_PI / (n - 1);
                glm::vec3 p= (bezier.eval(u));
                p.z = cos(theta) *p.y;
                p.y = sin(theta) *p.y;
                poses[(i * n) + j] = q+(p-q);
            }
        }


        std::vector<BaseMesh::vertexData> data;
        std::vector<uint32_t> indices;
        for (auto &pos: poses) {
            data.emplace_back(
                    pos,
                    glm::vec3(0.0),
                    glm::vec3(0.0),
                    glm::vec2(0.0),
                    0
            );
        }
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - 1; ++j) {
                indices.push_back(i * n + j);
                indices.push_back((i) * n + j + 1);
                indices.push_back((i + 1) * n + j + 1);

                indices.push_back(i * n + j);
                indices.push_back((i + 1) * n + j + 1);
                indices.push_back((i + 1) * n + j);
            }
        }
        BaseMesh::SubMesh sub;
        sub.init(data, indices, true);
        auto res = std::make_shared<BaseMesh>();
        res->addSubMesh(sub);
        return res;
    }
}

