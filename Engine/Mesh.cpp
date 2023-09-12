//
// Created by Constantin on 19/07/2023.
//

#include <iostream>
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "bgfx/bgfx.h"
#include "assimp/postprocess.h"

using namespace Engine;

void BaseMesh::loadMesh(const std::string &Filename, bool simpleImport) {
    Assimp::Importer Importer;

    const aiScene *pScene = Importer.ReadFile(Filename.c_str(), simpleImport ? aiProcess_JoinIdenticalVertices
                                                                             : aiProcessPreset_TargetRealtime_MaxQuality);
    if (pScene) {
        std::cout << "reading mesh : " << Filename.c_str() << std::endl;
        initFromScene(pScene, Filename);
    } else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
        std::string error = "Error parsing ";
        error.append(Filename);
        error.append(" : ");
        error.append(Importer.GetErrorString());
    }
}

void BaseMesh::initFromScene(const aiScene *pScene, const std::string &Filename) {

    // Initialize the meshes in the scene one by one
    subMeshes.resize(pScene->mNumMeshes);

    std::cout << "init scene, size : " << subMeshes.size() << std::endl;
    for (unsigned int i = 0; i < subMeshes.size(); i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh);

        uint16_t offset = vertexesAllData.size();

        vertexesAllData.insert(
                vertexesAllData.end(),
                subMeshes[i].vertexesData.begin(),
                subMeshes[i].vertexesData.end());

        for (const auto &index: subMeshes[i].indices) {
            indicesAllData.emplace_back(offset + index);
        }


    }

    vly.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

    VBH = bgfx::createVertexBuffer(bgfx::makeRef(vertexesAllData.data(), vertexesAllData.size() * sizeof(vertexData)),
                                   vly);
    IBH = bgfx::createIndexBuffer(bgfx::makeRef(indicesAllData.data(), indicesAllData.size() * sizeof(uint16_t)));
}

void BaseMesh::initMesh(unsigned int Index, const aiMesh *paiMesh) {
    std::vector<vertexData> vertexesData;
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangent;
    std::vector<uint16_t> indexes;
    size_t size = paiMesh->mNumVertices;
    //std::cout << "init mesh : " << Index << " / vert : " << size << " / triangle : " << paiMesh->mNumFaces << std::endl;

    verts.reserve(size);
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

        vertexesData.emplace_back(
                glPos,
                glNorm,
                glTang,
                glTex,
                0
        );
        verts.emplace_back(pPos->x, pPos->y, pPos->z);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace &Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        indexes.push_back(Face.mIndices[0]);
        indexes.push_back(Face.mIndices[1]);
        indexes.push_back(Face.mIndices[2]);
    }
    subMeshes[Index].init(vertexesData, indexes, paiMesh->HasNormals());

}

BaseMesh::~BaseMesh() {
    bgfx::destroy(VBH);
    bgfx::destroy(IBH);
}


void
BaseMesh::SubMesh::init(const std::vector<vertexData> &Vertices, const std::vector<uint16_t> &Indices, bool hasNormal) {

    vertexesData = Vertices;
    indices = Indices;
    NumVertices = Vertices.size();


    if(!hasNormal){
        triangleAdjencyGenerated = true;

        std::map<std::pair<uint16_t, uint16_t>, std::pair<uint16_t, uint16_t>> triangleOppositeMap;


        vertexSingleAdjacency.reserve(vertexesData.size());
        for (int i = 0; i < vertexesData.size(); ++i) {
            vertexSingleAdjacency.emplace_back(0);
        }
        triangleAdjacency.reserve(indices.size());
        for (int i = 0; i < indices.size(); ++i) {
            triangleAdjacency.emplace_back(-1);
        }

        //Triangle adjacency
        for (int i = 0; i < indices.size() / 3; i++) {
            vertexSingleAdjacency[indices[i]] = i;
            vertexSingleAdjacency[indices[i + 1]] = i;
            vertexSingleAdjacency[indices[i + 2]] = i;


#pragma omp parallel for default(none) shared(i, triangleOppositeMap)
            for (int index = 0; index < 3; ++index) {
                int first = (index % 3);
                int second = (index + 1) % 3;
                int third = (index + 2) % 3;
                int indexedFirst = i * 3 + first;
                int indexedSecond = i * 3 + second;
                int indexedThird = i * 3 + third;
                {
                    std::pair<uint16_t, uint16_t> edge1 = std::make_pair(
                            std::min(indices[indexedFirst], indices[indexedSecond]),
                            std::max(indices[indexedFirst], indices[indexedSecond]));
                    if (!triangleOppositeMap.contains(edge1)) {
                        std::pair<uint16_t, uint16_t> triangleAdjData1 = std::make_pair(i * 3, third);
                        triangleOppositeMap[edge1] = triangleAdjData1;
                    } else {
                        uint16_t target = triangleOppositeMap[edge1].first;
                        uint16_t offset = triangleOppositeMap[edge1].second;
                        triangleAdjacency[indexedThird] = target;
                        triangleAdjacency[target + offset] = i * 3;
                    }
                }
            }
        }
    }
    else {
        triangleAdjencyGenerated = false;
    }



#ifndef NDEBUG
    if (vertexesData.size() < 16) {
        auto It = vertexBegin();
        auto endIt = vertexEnd();
        for (; It != endIt; It++) {
            std::cout << It->position.x << " " << It->position.y << " " << It->position.z << std::endl;
        }
    }
#endif


    vly.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

    VBH = bgfx::createVertexBuffer(bgfx::makeRef(vertexesData.data(), vertexesData.size() * sizeof(vertexData)), vly);
    IBH = bgfx::createIndexBuffer(bgfx::makeRef(indices.data(), indices.size() * sizeof(uint16_t)));
}

BaseMesh::SubMesh::~SubMesh() {
    bgfx::destroy(VBH);
    bgfx::destroy(IBH);
}
