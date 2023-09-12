//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <vector>
#include <array>
#include "assimp/scene.h"
#include "bx/math.h"
#include "glm/glm.hpp"
#include "bgfx/bgfx.h"
#include "Components/CTransform.h"

namespace Engine {
    class BaseMesh {
    private:
        struct vertexData {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 texCoord;
            uint32_t m_abgr;
        };

        struct SubMesh {
            SubMesh() = default;

            ~SubMesh();

            void init(const std::vector<vertexData> &Vertices, const std::vector<uint16_t> &Indices, bool hasNormal);
            
            std::vector<vertexData> vertexesData;
            std::vector<uint16_t> indices; // 3-point triangle indices
            std::vector<uint16_t> vertexSingleAdjacency; //indice of the first point of the triangle triangle access, same indice as vertexData
            std::vector<int> triangleAdjacency; //Indice of the first point of every adjacent triangle, same indice as indices vector

            bool triangleAdjencyGenerated;

            bgfx::VertexLayout vly;
            bgfx::VertexBufferHandle VBH;
            bgfx::IndexBufferHandle IBH;
            unsigned int NumVertices;
            unsigned int MaterialIndex;




            class vertexIterator {
                using iterator_category = std::forward_iterator_tag;
                using difference_type   = std::ptrdiff_t;
                using value_type        = std::vector<vertexData>::iterator;
                using pointer           = std::vector<vertexData>::iterator*;  // or also value_type*
                using reference         = std::vector<vertexData>::iterator&;  // or also value_type&
                std::vector<vertexData>::iterator m_it;
            public:
                explicit vertexIterator(std::vector<vertexData>::iterator it) : m_it(it) {}

                vertexIterator &operator++() {
                    m_it++;
                    return *this;
                }

                vertexIterator operator++(int) {
                    vertexIterator retval = *this;
                    ++(*this);
                    return retval;
                }

                bool operator==(const vertexIterator &b) const {
                    return b.m_it != m_it;
                }

                bool operator!=(vertexIterator &other) const { return m_it != other.m_it; }

                vertexData &operator*() const {
                    return *m_it;
                }

                vertexData *operator->() {
                    return &*m_it;
                }
            };

            vertexIterator vertexBegin() {
                return vertexIterator(vertexesData.begin());
            }

            vertexIterator vertexEnd() {
                return vertexIterator(vertexesData.end());
            }
            
            
            
        };

        bgfx::VertexLayout vly;

    public:
        ~BaseMesh();

        std::vector<SubMesh> subMeshes;

        void loadMesh(const std::string &Filename, bool simpleImport = false);
        std::vector<vertexData> vertexesAllData;
        std::vector<uint16_t> indicesAllData;
        bgfx::VertexBufferHandle VBH;
        bgfx::IndexBufferHandle IBH;

    private:
        void initFromScene(const aiScene *pScene, const std::string &Filename);

        void initMesh(unsigned int Index, const aiMesh *paiMesh);

    };

    class SkeletalMesh : public BaseMesh {

    };

    class StaticMesh : public BaseMesh {

    };

}


#endif //ENGINE_MESH_H
