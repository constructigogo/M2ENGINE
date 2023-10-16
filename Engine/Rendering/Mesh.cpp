//
// Created by Constantin on 19/07/2023.
//

#include <iostream>
#include "../Core/Log.h"
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "bgfx/bgfx.h"
#include "assimp/postprocess.h"

using namespace Engine;

void BaseMesh::loadMesh(const std::string &Filename, bool simpleImport) {
    Assimp::Importer Importer;
    name = Filename;
    const aiScene *pScene = Importer.ReadFile(Filename.c_str(),
                                              simpleImport ? aiProcess_JoinIdenticalVertices |
                                                             aiProcess_GenBoundingBoxes | aiProcess_Triangulate
                                                           : aiProcessPreset_TargetRealtime_MaxQuality);
    if (pScene) {
        ENGINE_TRACE("Reading mesh " + Filename);
        initFromScene(pScene, Filename);
    } else {
        //printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
        std::string error = "Error parsing ";
        error.append(Filename);
        error.append(" : ");
        error.append(Importer.GetErrorString());
        ENGINE_WARN(error);
    }
}

void BaseMesh::initFromScene(const aiScene *pScene, const std::string &Filename) {
    if (pScene->mNumMeshes == 1) {
        ENGINE_TRACE("Init mesh as single");
        initMeshAsSingle(pScene->mMeshes[0]);
        return;
    }
    // Initialize the meshes in the scene one by one
    subMeshes.resize(pScene->mNumMeshes);

    //std::cout << "init mesh, size : " << subMeshes.size() << std::endl;
    ENGINE_TRACE("Init Mesh from scene, size : " + std::to_string(subMeshes.size()));
    for (unsigned int i = 0; i < subMeshes.size(); i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh);

        uint32_t offset = vertexesAllData.size();

        vertexesAllData.insert(
                vertexesAllData.end(),
                subMeshes[i].vertexesData.begin(),
                subMeshes[i].vertexesData.end());

        for (const auto &index: subMeshes[i].indices) {
            indicesAllData.emplace_back(offset + index);
        }


    }

    VBH = bgfx::createVertexBuffer(bgfx::makeRef(vertexesAllData.data(), vertexesAllData.size() * sizeof(vertexData)),
                                   getVLY());
    IBH = bgfx::createIndexBuffer(bgfx::makeRef(indicesAllData.data(), indicesAllData.size() * sizeof(uint32_t)),
                                  BGFX_BUFFER_INDEX32);
}

void BaseMesh::initMesh(unsigned int Index, const aiMesh *paiMesh) {
    std::vector<vertexData> vertexesData;
    std::vector<uint32_t> indexes;
    size_t size = paiMesh->mNumVertices;
    //std::cout << "init mesh : " << Index << " / vert : " << size << " / triangle : " << paiMesh->mNumFaces << std::endl;

    vertexesData.resize(size);
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < size; ++i) {
        const aiVector3D *pPos = &(paiMesh->mVertices[i]);
        const aiVector3D *pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D *pTang = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
        const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        glm::vec3 glPos(pPos->x, pPos->y, pPos->z);
        glm::vec3 glNorm(pNormal->x, pNormal->y, pNormal->z);
        glm::vec3 glTang(pTang->x, pTang->y, pTang->z);
        glm::vec2 glTex(pTexCoord->x, pTexCoord->y);

        vertexesData[i].position = glPos;
        vertexesData[i].normal = -glNorm;
        vertexesData[i].tangent = -glTang;
        vertexesData[i].texCoord = glTex;
        vertexesData[i].m_abgr = 0;

        /*
        vertexesData.emplace_back(
                glPos,
                glNorm,
                glTang,
                glTex,
                0
        );*/
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace &Face = paiMesh->mFaces[i];
        if (Face.mNumIndices < 3) {
            continue;
        }
        assert(Face.mNumIndices == 3);
        indexes.push_back(Face.mIndices[0]);
        indexes.push_back(Face.mIndices[1]);
        indexes.push_back(Face.mIndices[2]);
    }
    subMeshes[Index].init(vertexesData, indexes, paiMesh->HasNormals());
}

BaseMesh::~BaseMesh() {
    ENGINE_DEBUG("Cleaning Mesh " + name);
    if (bgfx::isValid(VBH)) {
        bgfx::destroy(VBH);
    }
    if (bgfx::isValid(IBH)) {
        bgfx::destroy(IBH);
    }
}

void BaseMesh::addSubMesh(BaseMesh::SubMesh &toAdd) {
    subMeshes.push_back(toAdd);

    vertexesAllData = toAdd.vertexesData;

    VBH = bgfx::createVertexBuffer(
            bgfx::copy(toAdd.vertexesData.data(), toAdd.vertexesData.size() * sizeof(vertexData)), getVLY());
    IBH = bgfx::createIndexBuffer(bgfx::copy(toAdd.indices.data(), toAdd.indices.size() * sizeof(uint32_t)),
                                  BGFX_BUFFER_INDEX32);

}

const std::string &BaseMesh::getName() const {
    return name;
}

void BaseMesh::initMeshAsSingle(const aiMesh *paimesh) {
    subMeshes.resize(1);
    initMesh(0, paimesh);

    auto current = subMeshes[0];
    auto aabb = paimesh->mAABB;
    glm::vec3 min(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z);
    glm::vec3 max(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z);
    boundingBox = Box(min, max);

    VBH = bgfx::createVertexBuffer(
            bgfx::copy(current.vertexesData.data(), current.vertexesData.size() * sizeof(vertexData)), getVLY());
    IBH = bgfx::createIndexBuffer(bgfx::copy(current.indices.data(), current.indices.size() * sizeof(uint32_t)),
                                  BGFX_BUFFER_INDEX32);
}

BaseMesh::BaseMesh(const std::string &name) : name(name) {}

void BaseMesh::refreshBuffers() {
    ENGINE_TRACE("Init Mesh from scene, size : " + std::to_string(subMeshes.size()));
    vertexesAllData.clear();
    indicesAllData.clear();
    for (auto &subMeshe: subMeshes) {
        uint32_t offset = vertexesAllData.size();

        vertexesAllData.insert(
                vertexesAllData.end(),
                subMeshe.vertexesData.begin(),
                subMeshe.vertexesData.end());

        for (const auto &index: subMeshe.indices) {
            indicesAllData.emplace_back(offset + index);
        }


    }

    VBH = bgfx::createVertexBuffer(bgfx::makeRef(vertexesAllData.data(), vertexesAllData.size() * sizeof(vertexData)),
                                   getVLY());
    IBH = bgfx::createIndexBuffer(bgfx::makeRef(indicesAllData.data(), indicesAllData.size() * sizeof(uint32_t)),
                                  BGFX_BUFFER_INDEX32);
}


void
BaseMesh::SubMesh::init(std::vector<vertexData> &Vertices, const std::vector<uint32_t> &Indices, bool hasNormal) {

    vertexesData = std::move(Vertices);
    indices = Indices;
    NumVertices = vertexesData.size();


    if (!hasNormal) {
        ENGINE_TRACE("Generating adjacency");
        triangleAdjencyGenerated = true;

        //key : Lower edge id first, then highest
        //val : first triangle id found with this edge, then opposite offset
        std::map<std::pair<uint32_t, uint32_t>, std::pair<uint32_t, uint32_t>> triangleOppositeMap;


        vertexSingleAdjacency.reserve(vertexesData.size());
        for (int i = 0; i < vertexesData.size(); ++i) {
            vertexSingleAdjacency.emplace_back(0);
        }
        triangleAdjacency.reserve(indices.size());
        for (int i = 0; i < indices.size(); ++i) {
            triangleAdjacency.emplace_back(-1);
        }

        ENGINE_DEBUG("Generating triangle adjacency");
        //Triangle adjacency
        for (int i = 0; i < indices.size() / 3; i++) {

            vertexSingleAdjacency[indices[i * 3]] = i;
            vertexSingleAdjacency[indices[i * 3 + 1]] = i;
            vertexSingleAdjacency[indices[i * 3 + 2]] = i;

            for (int index = 0; index < 3; ++index) {
                int first = (index % 3);
                int second = (index + 1) % 3;
                int third = (index + 2) % 3;
                int indexedFirst = i * 3 + first;
                int indexedSecond = i * 3 + second;
                int indexedThird = i * 3 + third;
                {
                    std::pair<uint32_t, uint32_t> edge1 = std::make_pair(
                            std::min(indices[indexedFirst], indices[indexedSecond]),
                            std::max(indices[indexedFirst], indices[indexedSecond]));
                    if (!triangleOppositeMap.contains(edge1)) {
                        std::pair<uint32_t, uint32_t> triangleAdjData1 = std::make_pair(i * 3, third);
                        triangleOppositeMap[edge1] = triangleAdjData1;
                    } else {
                        uint32_t target = triangleOppositeMap[edge1].first;
                        uint32_t offset = triangleOppositeMap[edge1].second;
                        triangleAdjacency[indexedThird] = target;
                        triangleAdjacency[target + offset] = i * 3;
                    }
                }
            }
        }
        ENGINE_DEBUG("Vertex fast access size : " + std::to_string(vertexSingleAdjacency.size()));
        ENGINE_DEBUG("Triangle adjacency size (1 per triangle but 3 data) : " +
                     std::to_string(triangleAdjacency.size()));
    } else {
        triangleAdjencyGenerated = false;
    }


#ifndef NDEBUG
    if (vertexesData.size() < 16 && false) {
        auto It = vertexBegin();
        auto endIt = vertexEnd();
        for (; It != endIt; It++) {
            std::cout << It->position.x << " " << It->position.y << " " << It->position.z << std::endl;
        }

        auto t_it = circulatorBegin(0);
        auto t_endIt = circulatorEnd(0);

        std::cout << std::endl;
        for (int i = 0; i < triangleAdjacency.size(); i += 3) {
            std::cout << "triangle index : " << i
                      << " : " << triangleAdjacency[i] << " " << triangleAdjacency[i + 1] << " "
                      << triangleAdjacency[i + 2]
                      << " : " << indices[i] << " " << indices[i + 1] << " " << indices[i + 2]
                      << std::endl;
        }

        std::cout << "vertex fast access" << std::endl;
        for (int i = 0; i < vertexSingleAdjacency.size(); i += 1) {
            std::cout << "vertex index : " << i << " : " << vertexSingleAdjacency[i] * 3 << std::endl;
        }
        std::cout << "with it : " << std::endl;

        for (; t_it != t_endIt; ++t_it) {
            //++t_it;
            uint32_t val = (*t_it);
            std::cout << val << std::endl;
            for (uint32_t i = 0; i < 3; ++i) {
                vertexesData[val + i].m_abgr = 0xFF0000FF;
            }
        }
        vertexesData[0].m_abgr = 0x00FF00FF;

    } else {
        splitTriangleCompactId(0, 0.2, 0.2);
        auto t_it = circulatorBegin(8);
        auto t_endIt = circulatorEnd(8);
        std::cout << "fast access : " << vertexSingleAdjacency[0] * 3 << std::endl;
        std::cout << "test 0 : " << indices[vertexSingleAdjacency[0] * 3] << std::endl;
        std::cout << "test 1 : " << indices[vertexSingleAdjacency[0] * 3 + 1] << std::endl;
        std::cout << "test 2 : " << indices[vertexSingleAdjacency[0] * 3 + 2] << std::endl;




        for (; t_it != t_endIt; ++t_it) {
            //++t_it;
            uint32_t val = (*t_it);
            ENGINE_DEBUG(val);
            //std::cout << val << std::endl;
            for (uint32_t i = 0; i < 3; ++i) {
                //std::cout << "indice : "<< indices[val+i] <<std::endl;
                vertexesData[indices[val + i]].m_abgr = 0xFF0000FF;
            }
        }
        vertexesData[8].m_abgr = 0xFF00FF00;
        //flipEdgeCompactId(0, 1);
        //flipEdgeCompactId(1, 2);
        //flipEdgeCompactId(4, 5);

        computeLaplacian();

    }
#endif

}

void BaseMesh::SubMesh::flipEdgeCompactId(uint32_t t1_id, uint32_t t2_id) {
    flipEdge(t1_id * 3, t2_id * 3);
}

void BaseMesh::SubMesh::flipEdge(uint32_t t1_id, uint32_t t2_id) {
    bool isAdj = false;
    std::pair<uint32_t, uint32_t> commonEdge;
    uint32_t oppositeID;
    for (int i = 0; i < 3; ++i) {
        int first = (i % 3);
        uint32_t indexedFirst = t1_id + first;
        oppositeID = triangleAdjacency[indexedFirst];
        if (oppositeID == t2_id) {
            int second = (i + 1) % 3;
            int third = (i + 2) % 3;
            uint32_t indexedSecond = t1_id + second;
            uint32_t indexedThird = t1_id + third;
            isAdj = true;
            commonEdge = std::make_pair(
                    std::min(indices[indexedSecond], indices[indexedThird]),
                    std::max(indices[indexedSecond], indices[indexedThird]));
            break;
        }
    }
    if (!isAdj) {
        ENGINE_DEBUG("Edges not adj");
        return;
    }
    //ENGINE_DEBUG("Flipping edge");
    ENGINE_DEBUG("Common edge : " +
                 std::to_string(t1_id) + "^" + std::to_string(commonEdge.first) +
                 "---" +
                 std::to_string(commonEdge.second) + "^" + std::to_string(t2_id));

    uint32_t oppositeVT1 = getOppositeFromEdge(t1_id, commonEdge.first, commonEdge.second);
    uint32_t oppositeVT2 = getOppositeFromEdge(t2_id, commonEdge.first, commonEdge.second);

    ENGINE_DEBUG(oppositeVT1);
    ENGINE_DEBUG(oppositeVT2);


}

int BaseMesh::SubMesh::getOppositeFromEdge(uint32_t t, uint32_t v1, uint32_t v2) {
    for (int i = 0; i < 3; ++i) {
        int first = (i % 3);
        int second = (i + 1) % 3;
        int third = (i + 2) % 3;
        uint32_t indexedFirst = t + first;
        uint32_t indexedSecond = t + second;
        uint32_t indexedThird = t + third;
        if (std::min(v1, v2) == std::min(indices[indexedFirst], indices[indexedSecond]) &&
            std::max(v1, v2) == std::max(indices[indexedFirst], indices[indexedSecond])
                ) {
            return (uint32_t) indexedThird;
        }
    }
    ENGINE_WARN("SubMesh::getOppositeFromEdge : Opp not found, check common edge");
    return -1;
}

std::pair<int,int> BaseMesh::SubMesh::getEdgeFromOpposite(uint32_t t, uint32_t v) {
    for (int i = 0; i < 3; ++i) {
        int first = (i % 3);
        int second = (i + 1) % 3;
        int third = (i + 2) % 3;
        uint32_t indexedFirst = t + first;
        uint32_t indexedSecond = t + second;
        uint32_t indexedThird = t + third;
        if(indices[indexedFirst] == v){
            return std::make_pair(std::min(indices[indexedSecond], indices[indexedThird]),std::max(indices[indexedSecond], indices[indexedThird]));
        }
    }
    ENGINE_WARN("SubMesh::getEdgeFromOpposite : Edge not found, check opposite indice");

    return std::make_pair(-1,-1);
}

void BaseMesh::SubMesh::computeLaplacian() {
    for (uint32_t i = 0; i < vertexesData.size(); ++i) {
        computeLaplacianLocal(i);
    }
}

void BaseMesh::SubMesh::computeLaplacianLocal(uint32_t v) {
    uint32_t root = v;
    auto it_begin = circulatorBegin(v);
    auto it_end = circulatorEnd(v);
    for (; it_begin!=it_end ; ++it_begin) {
        auto triangle_root = it_begin.getAroundID();
        auto [p1,p2] = getEdgeFromOpposite(*it_begin,triangle_root);
        if(p1==-1){
            continue;
        }


    }
}

void BaseMesh::SubMesh::splitTriangleCompactId(uint32_t tId, float u, float v) {
    splitTriangle(tId * 3, u, v);
}

void BaseMesh::SubMesh::splitTriangle(uint32_t tId, float u, float v) {
    if (u > 1.0 || v > 1.0 || u < 0.0 || v < 0.0 || u + v > 1.0) {
        return;
    }
    uint32_t oppId1 = triangleAdjacency[tId];
    uint32_t oppId2 = triangleAdjacency[tId + 1];
    uint32_t oppId3 = triangleAdjacency[tId + 2];

    uint32_t aId = indices[tId];
    uint32_t bId = indices[tId + 1];
    uint32_t cId = indices[tId + 2];
    glm::vec3 a = vertexesData[indices[tId]].position;
    glm::vec3 b = vertexesData[indices[tId + 1]].position;
    glm::vec3 c = vertexesData[indices[tId + 2]].position;

    glm::vec3 AB = b - a;
    glm::vec3 AC = c - a;

    std::pair<uint32_t, uint32_t> edge1 = std::make_pair(aId, bId);
    std::pair<uint32_t, uint32_t> edge2 = std::make_pair(bId, cId);
    std::pair<uint32_t, uint32_t> edge3 = std::make_pair(cId, aId);

    glm::vec3 p = a * (1 - u - v) + b * u + c * v;

    uint32_t otherOpposite2 = getOppositeFromEdge(oppId1, bId, cId);
    uint32_t otherOpposite3 = getOppositeFromEdge(oppId2, cId, aId);

    vertexData data{};
    data.position = p;
    vertexesData.push_back(data);
    uint32_t nId = vertexesData.size() - 1;
    uint32_t t1id = tId;
    indices[tId + 2] = nId;

    uint32_t t2id = indices.size();
    indices.push_back(bId);
    indices.push_back(cId);
    indices.push_back(nId);

    uint32_t t3id = indices.size();
    indices.push_back(cId);
    indices.push_back(aId);
    indices.push_back(nId);

    triangleAdjacency[otherOpposite2] = t2id;
    triangleAdjacency[otherOpposite3] = t3id;
    triangleAdjacency[t1id] = t2id;
    triangleAdjacency[t1id + 1] = t3id;
    //triangleAdjacency[tId + 2] = oppId3;

    triangleAdjacency.push_back(t3id);
    triangleAdjacency.push_back(t1id);
    triangleAdjacency.push_back(oppId1);

    triangleAdjacency.push_back(t1id);
    triangleAdjacency.push_back(t2id);
    triangleAdjacency.push_back(oppId2);

    vertexSingleAdjacency[aId] = t3id/3;
    vertexSingleAdjacency[bId] = t2id/3;
    vertexSingleAdjacency[cId] = t3id/3;
    vertexSingleAdjacency.push_back(t3id/3);

}

float BaseMesh::SubMesh::getTriangleAreaCompactId(uint32_t tId) {
    return getTriangleArea(tId * 3);
}

float BaseMesh::SubMesh::getTriangleArea(uint32_t tId) {
    return 0;
}













