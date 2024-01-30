//
// Created by Constantin on 23/07/2023.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include <utility>
#include "Data.h"
#include "Log.h"
#include "bimg/bimg.h"
#include "bx/readerwriter.h"
#include "../Components/CTransform.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../Components/CMeshRenderer.h"
#include "bimg/decode.h"
#include "Image.h"


using namespace Engine;

bx::FileReaderI *Data::s_fileReader = nullptr;
bx::AllocatorI *Data::allocator = nullptr;
bool Data::isCached = false;

std::unordered_map<std::string, std::shared_ptr<BaseMesh>> Engine::Data::meshCache;
std::unordered_map<std::string, std::shared_ptr<Texture>> Engine::Data::textureCache;
std::unordered_map<std::string, bgfx::ProgramHandle> Engine::Data::programCache;
std::unordered_map<std::string, bgfx::TextureHandle> Engine::Data::textureCacheRaw;
std::unordered_set<std::string> Engine::Data::loadableMesh;


std::shared_ptr<BaseMesh> Engine::Data::loadMesh(const std::string &fileName, bool simpleImport) {
    isCached=false;
    if (meshCache.contains(fileName)) {
        ENGINE_TRACE("Loading mesh " + fileName + " from cache");
        return meshCache[fileName];
    } else {
        ENGINE_TRACE("Loading mesh " + fileName);
        auto created = std::make_shared<BaseMesh>();
        created->loadMesh(fileName, simpleImport);
        meshCache[fileName] = created;
        return created;
    }
}


bgfx::ShaderHandle Engine::Data::loadShaderBin(const char *_name) {
    //char *data = new char[4096];
    std::vector<char> data;
    std::string path = "shaders/generated/";
    path.append(_name);
    path.append(".bin");

    std::ifstream file;
    size_t fileSize;

    file.open(path);
    bool success = false;
    if (file.is_open()) {
        file.unsetf(std::ios::skipws);
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        data.reserve(fileSize);
        file.seekg(0, std::ios::beg);
        data.insert(data.begin(),
                    std::istream_iterator<char>(file),
                    std::istream_iterator<char>());
        //file.read(data, fileSize);
        file.close();
        success = true;
    }
    if (!success) {
        std::cout << "could not load shader " << _name << std::endl;
    }

    const bgfx::Memory *mem = bgfx::copy(data.data(), fileSize + 1);
    mem->data[mem->size - 1] = '\0';
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, path.c_str());
    //delete[] data;
    return handle;
}

void Data::init() {
    static bx::DefaultAllocator s_allocator;
    allocator = &s_allocator;
    s_fileReader = BX_NEW(allocator, FileReader);

    const std::filesystem::path data{"data"};
    for (auto &file_it: std::filesystem::directory_iterator({data})) {
        const std::filesystem::path &file(file_it);
        auto ext = file.extension();
        if (ext == ".off" || ext == ".obj") {
            std::cout << file << '\n';
            Data::loadableMesh.emplace(file);
        }
    }
}

void *Data::load(const char *_name, uint32_t *_size) {
    if (bx::open(s_fileReader, _name)) {
        auto size = (uint32_t) bx::getSize(s_fileReader);
        void *data = bx::alloc(allocator, size);
        bx::read(s_fileReader, data, size, bx::ErrorAssert{});
        bx::close(s_fileReader);
        if (nullptr != _size) {
            *_size = size;
        }
        return data;
    }
    return nullptr;
}

void Data::imageReleaseCb(void *_ptr, void *_userData) {
    BX_UNUSED(_ptr);
    auto *imageContainer = (bimg::ImageContainer *) _userData;
    bimg::imageFree(imageContainer);
}

void Data::cleanup() {
    for (auto &[name, texHandle]: textureCacheRaw) {
        bgfx::destroy(texHandle);
    }
}

const std::unordered_set<std::string> &Data::getLoadableMesh() {
    return loadableMesh;
}

std::shared_ptr<Texture> Data::loadTexture(const char *_name, Texture::TYPE type) {

    if (textureCache.contains(_name)) {
        ENGINE_TRACE("Loading Texture " + std::string(_name) + " from cache");
        return textureCache[_name];
    }
    ENGINE_TRACE("Loading Texture " + std::string(_name));
    int w = 0, h = 0;

    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
    uint64_t _flags = 0;
    uint32_t size;
    void *data = load(_name, &size);
    if (data != nullptr) {
        bimg::ImageContainer *imageContainer = bimg::imageParse(allocator, data, size);

        if (imageContainer != nullptr) {
            const bgfx::Memory *mem = bgfx::makeRef(
                    imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer
            );
            bx::free(allocator, data);


            if (imageContainer->m_cubeMap) {
                handle = bgfx::createTextureCube(
                        uint16_t(imageContainer->m_width), 1 < imageContainer->m_numMips, imageContainer->m_numLayers,
                        bgfx::TextureFormat::Enum(imageContainer->m_format), _flags, mem
                );
            } else if (1 < imageContainer->m_depth) {
                handle = bgfx::createTexture3D(
                        uint16_t(imageContainer->m_width), uint16_t(imageContainer->m_height),
                        uint16_t(imageContainer->m_depth), 1 < imageContainer->m_numMips,
                        bgfx::TextureFormat::Enum(imageContainer->m_format), _flags, mem
                );
            } else if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers,
                                            bgfx::TextureFormat::Enum(imageContainer->m_format), _flags)) {
                handle = bgfx::createTexture2D(
                        uint16_t(imageContainer->m_width), uint16_t(imageContainer->m_height),
                        1 < imageContainer->m_numMips, imageContainer->m_numLayers,
                        bgfx::TextureFormat::Enum(imageContainer->m_format), _flags, mem
                );
                w = imageContainer->m_width;
                h = imageContainer->m_height;
            }
        }

        if (bgfx::isValid(handle)) {
            bgfx::setName(handle, _name);
        }
    }
    std::string name(_name);
    auto buff = std::make_shared<Texture>(name, w, h, handle, type);

    textureCache[_name] = buff;
    return buff;
}

std::vector<Object *> Data::loadScene(const std::string &fileName) {
    Assimp::Importer Importer;
    std::vector<Object *> res;
    ENGINE_TRACE("Try Read Scene " + fileName);
    const aiScene *pScene = Importer.ReadFile(fileName.c_str(),
                                              aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
    auto root = new Object();
    root->setName(fileName);
    root->addComponent<CTransform>();
    res.push_back(root);

    auto textured = bgfx::createProgram(
            Data::loadShaderBin("v_textured.vert"),
            Data::loadShaderBin("f_textured.frag"),
            true
    );
    Material material("textured", textured);

    if (pScene) {
        ENGINE_TRACE("Loading scene," + fileName + " , size : " + std::to_string(pScene->mNumMeshes));
        isCached=false;
        for (int i = 0; i < pScene->mNumMeshes; i += 1) {
            const aiMesh *paiMesh = pScene->mMeshes[i];
            if (paiMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
                continue;
            }
            std::string name = fileName + std::to_string(i);
            ENGINE_TRACE("Loading Mesh " + name);

            auto obj = new Object(name);
            obj->setParent(root);

            auto transform = obj->addComponent<CTransform>();
            auto rend = obj->addComponent<CMeshRenderer>();

            auto mesh = std::make_shared<BaseMesh>();
            mesh->subMeshes.resize(1);
            mesh->initMeshAsSingle(paiMesh);

            rend->setMesh(mesh, STATIC, false);
            auto matIdx = paiMesh->mMaterialIndex;
            auto mat = pScene->mMaterials[matIdx];
            auto matInst = material.createInstance();
            rend->setMaterial(matInst);

            /*
            ENGINE_TRACE("Material instance from [" + material.getName() + "] as " +
                         std::string(pScene->mMaterials[matIdx]->GetName().C_Str()));
            aiString textureName;//Filename of the texture using the aiString assimp structure
            if (mat->GetTextureCount(aiTextureType_DIFFUSE)) {
                //Get the file name of the texture by passing the variable by reference again
                //Second param is 0, which is the first diffuse texture
                //There can be more diffuse textures but for now we are only interested in the first one
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
                std::string path = "data/" + std::string(textureName.data);
                std::string textureType = "diffuse ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                matInst->setTexture(Data::loadTexture(path.c_str(), Texture::TYPE::COLOR));
            } else {
                matInst->setTexture(Data::loadTexture("data/color_flat.png", Texture::TYPE::COLOR));
            }

            if (mat->GetTextureCount(aiTextureType_HEIGHT)) {
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), textureName);
                std::string path = "data/" + std::string(textureName.data);
                std::string textureType = "normal ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                matInst->setTexture(Data::loadTexture(path.c_str(), Texture::TYPE::NORMAL));
            } else {
                matInst->setTexture(Data::loadTexture("data/normal_flat.png", Texture::TYPE::NORMAL));
            }

            if (mat->GetTextureCount(aiTextureType_SPECULAR)) {
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), textureName);
                std::string path = "data/" + std::string(textureName.data);
                std::string textureType = "specular ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                matInst->setTexture(Data::loadTexture(path.c_str(), Texture::TYPE::SPECULAR));
            } else {
                matInst->setTexture(Texture::TYPE::SPECULAR,
                                    Data::loadTexture("data/color_flat.png", Texture::TYPE::COLOR));
            }


            // material index is the ID of the material you are interested in
            float shininess;
            if (AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess)) {
                shininess = 20.f;
            } else {
                matInst->setSpecColor({1.0, 1.0, 1.0});
                matInst->setSpecularStr(shininess);
            }
            */


            meshCache[name] = mesh;
            //res.push_back(obj);
        }


    } else {
        printf("Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
        std::string error = "Error parsing ";
        error.append(fileName);
        error.append(" : ");
        error.append(Importer.GetErrorString());
    }
    return res;
}

bgfx::ProgramHandle Data::loadProgram(const char *vertex, const char *fragment) {
    std::string cat = std::string(vertex) + std::string(fragment);
    if (programCache.contains(cat)) {
        return programCache[cat];
    } else {
        auto created = bgfx::createProgram(
                Data::loadShaderBin(vertex),
                Data::loadShaderBin(fragment),
                true
        );
        programCache[cat] = created;
        return created;
    }
}

void Data::exportToOBJ(std::shared_ptr<BaseMesh> &mesh, std::string filename) {
    std::ofstream myfile;
    myfile.open("saved/" + filename + ".obj");
    for (auto &vert: mesh->subMeshes[0].vertexesData) {
        const glm::vec3 &pos = vert.position;
        std::string line = "v ";
        line += std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z);
        myfile << line << "\n";
    }
    myfile << "\n";
    for (int i = 0; i < mesh->subMeshes[0].indices.size(); i += 3) {
        uint32_t v1 = mesh->subMeshes[0].indices[i] + 1;
        uint32_t v2 = mesh->subMeshes[0].indices[i + 1] + 1;
        uint32_t v3 = mesh->subMeshes[0].indices[i + 2] + 1;
        std::string line = "f ";
        line += std::to_string(v1) + " " + std::to_string(v2) + " " + std::to_string(v3);
        myfile << line << "\n";
    }
    myfile.close();
}

std::pair<bgfx::VertexBufferHandle, bgfx::IndexBufferHandle> Data::buildIndirectCache() {
    std::vector<BaseMesh::vertexData> allVerts;
    std::vector<uint32_t> allIndices;
    int vOffset=0;
    int iOffset=0;
    if(meshCache.empty()){
        return {BGFX_INVALID_HANDLE,BGFX_INVALID_HANDLE};
    }
    ENGINE_TRACE("Rebuilding Cache");
    for (auto &[name, mesh]: meshCache) {
        ENGINE_DEBUG("BAKING "+name);
        auto &verts = mesh->vertexesAllData;
        auto &indices = mesh->indicesAllData;
        allVerts.insert(allVerts.end(), verts.begin(), verts.end());
        for (auto &ind: indices) {
            allIndices.push_back(ind);
        }
        mesh->vOffsetInGlobal = vOffset;
        mesh->IOffsetInGlobal = iOffset;
        vOffset+=verts.size();
        iOffset+=indices.size();
        mesh->release();
    }
    bgfx::VertexBufferHandle vertHandle=BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle indHandle=BGFX_INVALID_HANDLE;
    if (bgfx::isValid(vertHandle)) {
        bgfx::destroy(vertHandle);
    }
    if (bgfx::isValid(indHandle)) {
        bgfx::destroy(indHandle);
    }

    vertHandle = bgfx::createVertexBuffer(
            bgfx::copy(allVerts.data(), allVerts.size() * sizeof(BaseMesh::vertexData)),
            BaseMesh::getVLY());
    indHandle = bgfx::createIndexBuffer(bgfx::copy(allIndices.data(), allIndices.size() * sizeof(uint32_t)),
                                        BGFX_BUFFER_INDEX32);
    ENGINE_TRACE("cache indices count : {}", allIndices.size());

    isCached=true;

    return std::make_pair(vertHandle, indHandle);
}

HeightField Data::loadHeightFieldFromImage(const std::string &fileName, float scale) {

    std::string name = "data/"+fileName;
    Image img(name);
    int w = img.getWidth();
    int h = img.getHeight();
    HeightField res(w,h,scale);
#pragma omp parallel for
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            res.at(x,y) = ((float)img.at(x,y).r)/255.0f;
            //res.setAt(img.at(x,y).r,x,y) ;
            //res.at(x,y) = 0;
        }
    }
    return res;
}

void Data::trackMesh(std::shared_ptr<BaseMesh> mesh, const std::string &name) {
    meshCache[name]=std::move(mesh);
    loadableMesh.emplace(name);
}

void Data::invalidateCache() {
    isCached=false;
}
