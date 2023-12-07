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

        void bind();
        void release();

        static bgfx::VertexLayout getVLY() {
            bgfx::VertexLayout vly;
            vly.begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
                    .add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float, true)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();
            return vly;
        }

        struct vertexData {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 texCoord;
            uint32_t m_abgr;
        };

        struct SubMesh {
            SubMesh() = default;

            ~SubMesh() = default;

            void init(std::vector<vertexData> &Vertices, const std::vector<uint32_t> &Indices, bool hasNormal);
            void generateSingleVertexNormal();

            float getTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);
            float getTriangleArea(uint32_t tId);
            float getTriangleAreaCompactId(uint32_t tId);

            void splitTriangle(uint32_t tId, float u = 0.5f, float v = 0.5f);
            void splitTriangleCompactId(uint32_t tId, float u = 0.5f, float v = 0.5f);

            void flipEdge(uint32_t t1_id, uint32_t t2_id);
            void flipEdgeCompactId(uint32_t t1_id, uint32_t t2_id);

            void localDelaunay(uint32_t vId);

            int getOppositeFromEdge(int t, uint32_t v1, uint32_t v2);
            std::pair<int,int>  getEdgeFromOpposite(int t, uint32_t v);

            bool faceOrientationTest2D(uint32_t t);
            inline bool faceOrientationTest2DCompactId(uint32_t t)
                {return faceOrientationTest2D(t * 3);}

            int facePointInclusionTest2D(uint32_t t, glm::vec3 position);
            int faceOrientationTest2DCompactIdCompactId(uint32_t t, glm::vec3 position)
                {return facePointInclusionTest2D(t * 3, position);}

            void computeLaplacian();
            void computeLaplacianLocal(uint32_t v);

            void makeDelaunay();
            bool delaunayLocal(uint32_t t1,uint32_t t2);
            bool delaunayLocal(uint32_t t);
            bool delaunayLocalCompactId(uint32_t t1,uint32_t t2);
            bool delaunayLocalCompactId(uint32_t t);
            bool inCircle(uint32_t t1, uint32_t v);
            bool inCircle(uint32_t a,uint32_t b,uint32_t c, uint32_t v);

            std::vector<vertexData> vertexesData;
            std::vector<uint32_t> indices; // 3-point triangle indices
            std::vector<uint32_t> vertexSingleAdjacency; //COMPACT !!!!!indice (value) of the first point of the triangle access, same indice as vertexData
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
                int aroundVertexID;
                uint32_t offset = 0;
                int currentTriangleID;
                bool startingPoint;
                const std::vector<uint32_t> &m_t;
                const std::vector<uint32_t> &m_vAdj;
                const std::vector<int> &m_tAdj;
            public:
                explicit triangleCirculator(int vertexID, const std::vector<uint32_t> &triangles,
                                            const std::vector<uint32_t> &vAdj, const std::vector<int> &tAdj, bool start)
                        : m_t(triangles), m_vAdj(vAdj), m_tAdj(tAdj) {
                    assert(vertexID < vAdj.size());
                    startingPoint = start;
                    aroundVertexID = vertexID;
                    currentTriangleID = m_vAdj[aroundVertexID] * 3;
                    ENGINE_DEBUG("STARTING ON TRIANGLE : " + std::to_string(currentTriangleID));
                }

                uint32_t getOffset() const {
                    return offset;
                }

                uint32_t getAroundID() const {
                    return aroundVertexID;
                }

                triangleCirculator &operator++() {
                    startingPoint = false;
                    uint32_t i = 0;
                    for (; i < 3; ++i) {
                        if (m_t[currentTriangleID + i] == aroundVertexID) {
                            offset = i;
                            break;
                        }
                    }
                    currentTriangleID = m_tAdj[currentTriangleID + ((i + 2) % 3)];
                    return *this;
                }

                triangleCirculator operator++(int) {
                    triangleCirculator retval = *this;
                    ++(*this);
                    return retval;
                }

                bool operator==(const triangleCirculator &b) const {
                    return !startingPoint && (b.currentTriangleID == currentTriangleID) && currentTriangleID!=-1;
                }

                bool operator!=(triangleCirculator &other) const {
                    return (startingPoint || (currentTriangleID != other.currentTriangleID)) && currentTriangleID!=-1 ;
                }


                int operator*() const {
                    return currentTriangleID;
                }

                int *operator->() {
                    return &currentTriangleID;
                }
            };

            triangleCirculator circulatorBegin(int index) {
                return triangleCirculator(index, indices, vertexSingleAdjacency, triangleAdjacency, true);
            }

            triangleCirculator circulatorEnd(int index) {
                return triangleCirculator(index, indices, vertexSingleAdjacency, triangleAdjacency, false);
            }


        };


        void addSubMesh(SubMesh &toAdd);

        void initMesh(unsigned int Index, const aiMesh *paiMesh);

        void initMeshAsSingle(const aiMesh *paimesh);

        std::vector<SubMesh> subMeshes;
        Box boundingBox = Box({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});


        void loadMesh(const std::string &Filename, bool simpleImport = false);

        void refreshBuffers();

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
