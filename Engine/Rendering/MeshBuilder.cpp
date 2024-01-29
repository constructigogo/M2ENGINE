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
        poses.resize(2 * (n - 1) * (n - 1));
        for (int i = 0; i < n; ++i) {
            float u = (float) i / float(n - 1);
            glm::vec3 q = axis.eval(u);
            for (int j = 0; j < n; ++j) {
                float theta = -2.0 * j * M_PI / (n - 1);
                glm::vec3 p = (bezier.eval(u));
                p.z = cos(theta) * p.y;
                p.y = sin(theta) * p.y;
                poses[(i * n) + j] = q + (p - q);
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

    std::shared_ptr<BaseMesh> MeshBuilder::Polygonize(const HeightField &scalarField) {
        size_t length = scalarField.length();
        std::vector<BaseMesh::vertexData> data(length);
        std::vector<uint32_t> indices;
        indices.reserve(length);
        int w = scalarField.gridWidth();
        int h = scalarField.gridHeight();
        ENGINE_TRACE("[MeshBuilder] : Creating Landscape single Mesh with res x:{} y:{}", w, h);

#pragma omp parallel for
        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < h; ++y) {
                int index = x + y * w;
                auto gradient = scalarField.gradientAt(x, y);
                data[index].position = glm::vec3(x - w / 2, scalarField.heightAt(x, y), y - h / 2);
                data[index].normal = scalarField.normalAt(x,y);
                data[index].texCoord = glm::vec2((float) x / w, (float) y / h);
                data[index].m_abgr = 0xFF000000 + (uint32_t) (0x00FFFFFF * scalarField.at(x, y));
            }
        }

        for (int y = 0; y < h - 1; ++y) {
            for (int x = 0; x < w - 1; ++x) {
                int index = x + y * w;
                //top left
                indices.push_back(index);
                indices.push_back(index + w);
                indices.push_back(index + 1);
                glm::vec3 norm = glm::normalize(glm::cross(
                        glm::normalize(data[index + w].position -
                        data[index].position),
                        glm::normalize(data[index + 1].position -
                        data[index].position)
                ));
                //data[index].normal = norm;
                //data[index + 1].normal = norm;
                //data[index + w].normal = norm;
                //lower right
                indices.push_back(index + 1);
                indices.push_back(index + w);
                indices.push_back(index + w + 1);
            }
        }


        BaseMesh::SubMesh mesh;
        mesh.init(data, indices, true);
        auto res = std::make_shared<BaseMesh>("Landscape");
        res->addSubMesh(mesh);
        res->boundingBox = Box(
                res->boundingBox.getLower() + glm::vec3(-w / 2, -1, -h / 2),
                res->boundingBox.getUpper() + glm::vec3(w / 2, 1, h / 2)
        );
        return res;
    }
}

