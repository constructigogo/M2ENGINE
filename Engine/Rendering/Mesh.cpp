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
                                                             aiProcess_GenBoundingBoxes
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


    }

    refreshBuffers();
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

    refreshBuffers();
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

    refreshBuffers();
}

BaseMesh::BaseMesh(const std::string &name) : name(name) {}

void BaseMesh::refreshBuffers() {
    release();
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
    bind();
}

void BaseMesh::bind() {
    if (!bgfx::isValid(VBH) && !bgfx::isValid(IBH)) {
        VBH = bgfx::createVertexBuffer(
                bgfx::makeRef(vertexesAllData.data(), vertexesAllData.size() * sizeof(vertexData)),
                getVLY());
        IBH = bgfx::createIndexBuffer(bgfx::makeRef(indicesAllData.data(), indicesAllData.size() * sizeof(uint32_t)),
                                      BGFX_BUFFER_INDEX32);
    }
}

void BaseMesh::release() {
    if (bgfx::isValid(VBH) && bgfx::isValid(IBH)) {
        bgfx::destroy(VBH);
        bgfx::destroy(IBH);
        VBH = BGFX_INVALID_HANDLE;
        IBH = BGFX_INVALID_HANDLE;
    }
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
        if (hasNormal) {
            return;
        }
        splitTriangleCompactId(0, 0.2, 0.2);

        //flipEdgeCompactId(0, 1);
        //flipEdgeCompactId(1, 2);
        //flipEdgeCompactId(4, 5);



        //computeLaplacian();


        //flipEdgeCompactId(1, 4);
        ENGINE_DEBUG(delaunayLocal(3, 12));

        makeDelaunay();
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
    uint32_t oppositeIndexed1 = getOppositeFromEdge(t1_id, commonEdge.first, commonEdge.second);
    uint32_t oppositeIndexed2 = getOppositeFromEdge(t2_id, commonEdge.first, commonEdge.second);
    uint32_t offset1 = oppositeIndexed1 - t1_id;
    uint32_t offset2 = oppositeIndexed2 - t2_id;

    uint32_t vID1 = indices[t1_id + (oppositeIndexed1 + 1) % 3];
    uint32_t vID2 = indices[oppositeIndexed1];
    uint32_t vID3 = indices[t1_id + (oppositeIndexed1 - 1) % 3];
    uint32_t vID4 = indices[oppositeIndexed2];

    int opp11 = triangleAdjacency[t1_id + (offset1 + 1) % 3];
    int opp23 = triangleAdjacency[t2_id + (offset2 + 1) % 3];
    int oppT1 = getOppositeFromEdge(opp11, vID2, vID3);
    int oppT2 = getOppositeFromEdge(opp23, vID1, vID4);


    indices[t1_id + (oppositeIndexed1 - 1) % 3] = vID4;
    indices[t2_id + (offset2 - 1) % 3] = vID2;


    triangleAdjacency[oppositeIndexed1] = opp23;
    triangleAdjacency[oppositeIndexed2] = opp11;

    triangleAdjacency[t1_id + (offset1 + 1) % 3] = t2_id;
    triangleAdjacency[t2_id + (offset2 + 1) % 3] = t1_id;

    if (oppT1 != -1)
        triangleAdjacency[oppT1] = t2_id;
    if (oppT2 != -1)
        triangleAdjacency[oppT2] = t1_id;

    vertexSingleAdjacency[indices[t1_id + (offset1 + 1) % 3]] = t1_id / 3;
    vertexSingleAdjacency[indices[t2_id + (offset2 + 1) % 3]] = t2_id / 3;
}

int BaseMesh::SubMesh::getOppositeFromEdge(int t, uint32_t v1, uint32_t v2) {
    if (t == -1) return t;
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
            return indexedThird;
        }
    }
    ENGINE_WARN("SubMesh::getOppositeFromEdge : Opp not found, check common edge");
    return -1;
}

std::pair<int, int> BaseMesh::SubMesh::getEdgeFromOpposite(int t, uint32_t v) {
    if (t == -1) return std::make_pair(-1, -1);
    for (int i = 0; i < 3; ++i) {
        int first = (i % 3);
        int second = (i + 1) % 3;
        int third = (i + 2) % 3;
        uint32_t indexedFirst = t + first;
        uint32_t indexedSecond = t + second;
        uint32_t indexedThird = t + third;
        if (indices[indexedFirst] == v) {
            return std::make_pair(std::min(indices[indexedSecond], indices[indexedThird]),
                                  std::max(indices[indexedSecond], indices[indexedThird]));
        }
    }
    ENGINE_WARN("SubMesh::getEdgeFromOpposite : Edge not found, check opposite indice");

    return std::make_pair(-1, -1);
}

bool BaseMesh::SubMesh::faceOrientationTest2D(uint32_t t) {
    glm::vec3 &p = vertexesData[t * 3].position;
    glm::vec3 &q = vertexesData[t * 3 + 1].position;
    glm::vec3 &r = vertexesData[t * 3 + 2].position;

    float det = (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);

    return det > 0 ? 1 : -1;
}

int BaseMesh::SubMesh::facePointInclusionTest2D(uint32_t t, glm::vec3 position) {
    glm::vec3 &a = vertexesData[t * 3].position;
    glm::vec3 &b = vertexesData[t * 3 + 1].position;
    glm::vec3 &c = vertexesData[t * 3 + 2].position;
    glm::vec3 &d = position;

    float surfaceT = getTriangleArea(a, b, c);
    if (surfaceT == 0)
        return -1;

    float surfaceST0 = getTriangleArea(d, a, b);
    float surfaceST1 = getTriangleArea(d, b, c);
    float surfaceST2 = getTriangleArea(d, c, a);

    if (surfaceST0 == 0 || surfaceST1 == 0 || surfaceST2 == 0)
        return 0;

    if (surfaceST0 > 0 && surfaceST1 > 0 && surfaceST2 > 0) {
        return surfaceST0 + surfaceST1 + surfaceST2;
    }

    if (surfaceST0 < 0 && surfaceST1 < 0 && surfaceST2 < 0) {
        return surfaceST0 * -1 + surfaceST1 * -1 + surfaceST2 * -1;
    }

    return -1;
}

void BaseMesh::SubMesh::computeLaplacian() {
//#pragma omp parallel for
    for (uint32_t i = 0; i < vertexesData.size(); ++i) {
        computeLaplacianLocal(i);
    }
}

void BaseMesh::SubMesh::computeLaplacianLocal(uint32_t v) {
    uint32_t root = v;
    auto it_begin = circulatorBegin(v);
    auto it_end = circulatorEnd(v);
    for (; it_begin != it_end; ++it_begin) {
        auto triangle_root = it_begin.getAroundID();
        auto [p1, p2] = getEdgeFromOpposite(*it_begin, triangle_root);
        if (p1 == -1) {

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
    int oppId1 = triangleAdjacency[tId];
    int oppId2 = triangleAdjacency[tId + 1];
    int oppId3 = triangleAdjacency[tId + 2];

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

    int otherOpposite2 = getOppositeFromEdge(oppId1, bId, cId);
    int otherOpposite3 = getOppositeFromEdge(oppId2, cId, aId);

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

    if (otherOpposite2 != -1)
        triangleAdjacency[otherOpposite2] = t2id;
    if (otherOpposite3 != -1)
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

    vertexSingleAdjacency[aId] = t3id / 3;
    vertexSingleAdjacency[bId] = t2id / 3;
    vertexSingleAdjacency[cId] = t3id / 3;
    vertexSingleAdjacency.push_back(t3id / 3);

}

float BaseMesh::SubMesh::getTriangleAreaCompactId(uint32_t tId) {
    return getTriangleArea(tId * 3);
}

float BaseMesh::SubMesh::getTriangleArea(glm::vec3 &a, glm::vec3 &b, glm::vec3 &c) {
    glm::vec3 AB = b - a;
    glm::vec3 AC = c - a;

    return glm::cross(AB, AC).z;
}

float BaseMesh::SubMesh::getTriangleArea(uint32_t tId) {
    glm::vec3 &a = vertexesData[indices[tId * 3]].position;
    glm::vec3 &b = vertexesData[indices[tId * 3 + 1]].position;
    glm::vec3 &c = vertexesData[indices[tId * 3 + 2]].position;
    return getTriangleArea(a, b, c);
}

void BaseMesh::SubMesh::localDelaunay(uint32_t vId) {

    auto triangle = vertexSingleAdjacency[vId];

    auto p1 = vertexesData[indices[triangle]].position;
    auto p2 = vertexesData[indices[triangle + 1]].position;
    auto p3 = vertexesData[indices[triangle + 2]].position;

    glm::vec3 normal = glm::cross(p2 - p1, p3 - p1);

}

bool BaseMesh::SubMesh::inCircle(uint32_t a, uint32_t b, uint32_t c, uint32_t v) {
    glm::vec3 pv = vertexesData[v].position;
    glm::vec3 pa = vertexesData[a].position - pv;
    glm::vec3 pb = vertexesData[b].position - pv;
    glm::vec3 pc = vertexesData[c].position - pv;
    float res = (
            (pa.x * pa.x + pa.y * pa.y) * (pb.x * pc.y - pc.x * pb.y) -
            (pb.x * pb.x + pb.y * pb.y) * (pa.x * pc.y - pc.x * pa.y) +
            (pc.x * pc.x + pc.y * pc.y) * (pa.x * pb.y - pb.x * pa.y)
    );
    return res > 0;
}

bool BaseMesh::SubMesh::delaunayLocal(uint32_t t1, uint32_t t2) {
    bool isAdj = false;
    std::pair<uint32_t, uint32_t> commonEdge;
    uint32_t offset = 0;
    uint32_t oppositeID;
    for (int i = 0; i < 3; ++i) {
        int first = (i % 3);
        uint32_t indexedFirst = t1 + first;
        oppositeID = triangleAdjacency[indexedFirst];
        if (oppositeID == t2) {
            int second = (i + 1) % 3;
            int third = (i + 2) % 3;
            uint32_t indexedSecond = t1 + second;
            uint32_t indexedThird = t1 + third;
            isAdj = true;
            offset = i;
            commonEdge = std::make_pair(
                    std::min(indices[indexedSecond], indices[indexedThird]),
                    std::max(indices[indexedSecond], indices[indexedThird]));
            break;
        }
    }
    if (!isAdj) {
        ENGINE_DEBUG("Edges not adj");
        return true;
    }
    uint32_t oppositeIndexed1 = getOppositeFromEdge(t1, commonEdge.first, commonEdge.second);
    uint32_t oppositeIndexed2 = getOppositeFromEdge(t2, commonEdge.first, commonEdge.second);

    bool delaunayT1 = inCircle(t1, indices[oppositeIndexed2]);
    bool delaunayT2 = inCircle(t2, indices[oppositeIndexed1]);

    return !(delaunayT1 || delaunayT2);
}

bool BaseMesh::SubMesh::inCircle(uint32_t t1, uint32_t v) {
    return inCircle(indices[t1], indices[t1 + 1], indices[t1 + 2], v);
}

bool BaseMesh::SubMesh::delaunayLocalCompactId(uint32_t t1, uint32_t t2) {
    return delaunayLocal(t1 * 3, t2 * 3);
}

void BaseMesh::SubMesh::makeDelaunay() {
    std::queue<std::pair<uint32_t, uint32_t>> checkQueue;
    for (int i = 0; i < indices.size(); i += 3) {
        for (int v = 0; v < 3; ++v) {
            int oppositeTriangleID = triangleAdjacency[i + v];
            if (oppositeTriangleID != -1) {
                checkQueue.emplace(i, oppositeTriangleID);
            }
        }
    }

    while (!checkQueue.empty()) {
        auto current = checkQueue.front();
        checkQueue.pop();
        if (!delaunayLocal(current.first, current.second)) {
            ENGINE_DEBUG("Flipping edge : " + std::to_string(current.first) + " / " + std::to_string(current.second));
            flipEdge(current.first, current.second);
            for (int i = 0; i < 3; ++i) {
                if (triangleAdjacency[current.first + i] != -1) {
                    checkQueue.emplace(current.first, triangleAdjacency[current.first + i]);
                }
                if (triangleAdjacency[current.second + i] != -1) {
                    checkQueue.emplace(current.second, triangleAdjacency[current.second + i]);
                }
            }
        }
    }

}

bool BaseMesh::SubMesh::delaunayLocal(uint32_t t) {
    return false;
}

bool BaseMesh::SubMesh::delaunayLocalCompactId(uint32_t t) {
    return false;
}













