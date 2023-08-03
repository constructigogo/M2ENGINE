//
// Created by Constantin on 19/07/2023.
//

#include <iostream>
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "bgfx/bgfx.h"
#include "assimp/postprocess.h"

using namespace Engine;

void BaseMesh::loadMesh(const std::string &Filename) {
    Assimp::Importer Importer;

    const aiScene *pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs |
                                                                aiProcess_JoinIdenticalVertices);
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

        for (const auto & index:subMeshes[i].indices) {
            indicesAllData.emplace_back(offset+index);
        }


    }

    vly.begin()
            .add(bgfx::Attrib::Position,3,bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,3,bgfx::AttribType::Float, true)
            .add(bgfx::Attrib::TexCoord0,2,bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();

    VBH = bgfx::createVertexBuffer(bgfx::makeRef(vertexesAllData.data(),vertexesAllData.size()*sizeof(vertexData)),vly);
    IBH = bgfx::createIndexBuffer(bgfx::makeRef(indicesAllData.data(),indicesAllData.size() *sizeof(uint16_t)));



}

void BaseMesh::initMesh(unsigned int Index, const aiMesh *paiMesh) {
    std::vector<vertexData> vertexesData;
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    std::vector<uint16_t> indexes;
    size_t size = paiMesh->mNumVertices;
    //std::cout << "init mesh : " << Index << " / vert : " << size << " / triangle : " << paiMesh->mNumFaces << std::endl;

    verts.reserve(size);
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);


    for (int i = 0; i < size; ++i) {
        const aiVector3D *pPos = &(paiMesh->mVertices[i]);
        const aiVector3D *pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        glm::vec3 glPos(pPos->x, pPos->y, pPos->z);
        glm::vec3 glNorm(pNormal->x, pNormal->y, pNormal->z);
        glm::vec2 glTex(pTexCoord->x, pTexCoord->y);

        vertexesData.emplace_back(
                glPos,
                glNorm,
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

    //subMeshes[Index].init(verts, indexes);
    subMeshes[Index].init(vertexesData, indexes);

}

BaseMesh::~BaseMesh() {
    bgfx::destroy(VBH);
    bgfx::destroy(IBH);
}


bool BaseMesh::SubMesh::init(const std::vector<vertexData> &Vertices, const std::vector<uint16_t> &Indices) {

    vertexesData = Vertices;
    indices = Indices;
    NumVertices = Vertices.size();

    vly.begin()
        .add(bgfx::Attrib::Position,3,bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal,3,bgfx::AttribType::Float, true)
        .add(bgfx::Attrib::TexCoord0,2,bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
        .end();

    VBH = bgfx::createVertexBuffer(bgfx::copy(vertexesData.data(),vertexesData.size()*sizeof(vertexData)),vly);
    IBH = bgfx::createIndexBuffer(bgfx::copy(indices.data(),indices.size() *sizeof(uint16_t)));


    return true;
}

BaseMesh::SubMesh::~SubMesh() {
    bgfx::destroy(VBH);
    bgfx::destroy(IBH);
}
