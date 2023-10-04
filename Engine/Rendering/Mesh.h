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
#include "../Components/CTransform.h"
#include "../Core/Box.h"
#include "../Core/Log.h"

namespace Engine {
    class BaseMesh {
    private:
        bgfx::VertexLayout vly;

        std::string name;
    public:
        BaseMesh() = default;

        explicit BaseMesh(const std::string &name);

        ~BaseMesh();

        const std::string &getName() const;


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

            void init(std::vector<vertexData> &Vertices, const std::vector<uint32_t> &Indices, bool hasNormal);

            std::vector<vertexData> vertexesData;
            std::vector<uint32_t> indices; // 3-point triangle indices
            std::vector<uint32_t> vertexSingleAdjacency; //indice (value) of the first point of the triangle access, same indice as vertexData
            std::vector<int> triangleAdjacency; //Indice (value) of the first point of every adjacent triangle, same indice as indices vector

            bool triangleAdjencyGenerated = false;

            bgfx::VertexLayout vly;
            bgfx::VertexBufferHandle VBH = BGFX_INVALID_HANDLE;
            bgfx::IndexBufferHandle IBH = BGFX_INVALID_HANDLE;
            unsigned int NumVertices;
            unsigned int MaterialIndex;


            class vertexIterator {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::vector<vertexData>::iterator;
                using pointer = std::vector<vertexData>::iterator *;  // or also value_type*
                using reference = std::vector<vertexData>::iterator &;  // or also value_type&
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

            class triangleCirculator {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::vector<int>::iterator;
                using pointer = std::vector<int>::iterator *;  // or also value_type*
                using reference = std::vector<int>::iterator &;  // or also value_type&
                int aroundVertexID;
                int offset = 1;
                uint32_t firstTriangleID;
                bool startingPoint;
                const std::vector<uint32_t>& m_t;
                const std::vector<uint32_t>& m_vAdj;
                const std::vector<int>& m_tAdj;
            public:
                explicit triangleCirculator(int vertexID,const std::vector<uint32_t>& triangles , const std::vector<uint32_t>& vAdj, const std::vector<int>& tAdj, bool start) : m_t(triangles), m_vAdj(vAdj), m_tAdj(tAdj){
                    startingPoint=start;
                    aroundVertexID = vertexID;
                    firstTriangleID = m_vAdj[aroundVertexID]*3;
                }


                triangleCirculator &operator++() {
                    startingPoint=false;
                    uint32_t i=0;
                    for (; i <3 ; ++i) {
                        if(m_t[firstTriangleID+i]==aroundVertexID){
                            break;
                        }
                    }
                    firstTriangleID=m_tAdj[firstTriangleID+((i+2)%3)];
                    return *this;
                }

                triangleCirculator operator++(int) {
                    triangleCirculator retval = *this;
                    ++(*this);
                    return retval;
                }

                bool operator==(const triangleCirculator &b) const {
                    return  !startingPoint&&(b.firstTriangleID == firstTriangleID);
                }

                bool operator!=(triangleCirculator &other) const { return startingPoint||(firstTriangleID != other.firstTriangleID); }


                uint32_t operator*() const {
                    return firstTriangleID;
                }

                uint32_t *operator->() {
                    return &firstTriangleID;
                }
            };

            triangleCirculator circulatorBegin(int index){
                return triangleCirculator(index,indices,vertexSingleAdjacency,triangleAdjacency,true);
            }

            triangleCirculator circulatorEnd(int index){
                return triangleCirculator(index,indices,vertexSingleAdjacency,triangleAdjacency,false);
            }


        };


        void addSubMesh(SubMesh &toAdd);

        void initMesh(unsigned int Index, const aiMesh *paiMesh);

        void initMeshAsSingle(const aiMesh *paimesh);

        std::vector<SubMesh> subMeshes;
        Box boundingBox = Box({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});


        void loadMesh(const std::string &Filename, bool simpleImport = false);

        std::vector<vertexData> vertexesAllData;
        std::vector<uint32_t> indicesAllData;
        bgfx::VertexBufferHandle VBH = BGFX_INVALID_HANDLE;
        bgfx::IndexBufferHandle IBH = BGFX_INVALID_HANDLE;

    private:
        void initFromScene(const aiScene *pScene, const std::string &Filename);


    };

    class SkeletalMesh : public BaseMesh {

    };

    class StaticMesh : public BaseMesh {

    };

}


#endif //ENGINE_MESH_H
