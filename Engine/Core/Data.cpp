//
// Created by Constantin on 23/07/2023.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include "Data.h"
#include "bimg/bimg.h"
#include "bx/readerwriter.h"
#include "../Components/CTransform.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../Components/CMeshRenderer.h"
#include "bimg/decode.h"


using namespace Engine;

bx::FileReaderI *Data::s_fileReader = nullptr;
bx::AllocatorI *Data::allocator = nullptr;


std::unordered_map<std::string, std::shared_ptr<BaseMesh>> Engine::Data::meshCache;
std::unordered_map<std::string, std::shared_ptr<Texture>> Engine::Data::textureCache;
std::unordered_map<std::string, bgfx::ProgramHandle> Engine::Data::programCache;
std::unordered_map<std::string, bgfx::TextureHandle> Engine::Data::textureCacheRaw;
std::unordered_set<std::string> Engine::Data::loadableMesh;


std::shared_ptr<BaseMesh> Engine::Data::loadMesh(const std::string &fileName, bool simpleImport) {
    if (meshCache.contains(fileName)) {
        return meshCache[fileName];
    } else {
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

bgfx::TextureHandle Data::loadTextureRaw(const char *_name) {
    if (textureCacheRaw.contains(_name)) {
        return textureCacheRaw[_name];
    }


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
            }

        }

        if (bgfx::isValid(handle)) {
            bgfx::setName(handle, _name);
        }
    }
    textureCacheRaw[_name] = handle;
    return handle;
}

void Data::init() {

    static bx::DefaultAllocator s_allocator;
    allocator = &s_allocator;
    s_fileReader = BX_NEW(allocator, FileReader);

    const std::filesystem::path data{"data"};
    for (auto &file_it: std::filesystem::directory_iterator({data}))
    {
        const std::filesystem::path& file(file_it);
        auto ext = file.extension();
        if(ext==".off" || ext == ".obj"){
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

std::shared_ptr<Texture> Data::loadTexture(const char *_name,Texture::TYPE type) {

    if (textureCache.contains(_name)) {
        return textureCache[_name];
    }
    int w=0,h=0;

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
                w=imageContainer->m_width;
                h=imageContainer->m_height;
            }

        }

        if (bgfx::isValid(handle)) {
            bgfx::setName(handle, _name);
        }
    }
    std::string name(_name);
    auto buff = std::make_shared<Texture>(name,w,h,handle,type);

    textureCache[_name] = buff;
    return buff;
}

std::vector<Object *> Data::loadScene(const std::string &fileName) {
    Assimp::Importer Importer;
    std::vector<Object *> res;
    const aiScene *pScene = Importer.ReadFile(fileName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
    auto root = new Object();
    root->setName(fileName);
    root->addComponent<CTransform>();
    res.push_back(root);

    auto textured = bgfx::createProgram(
            Data::loadShaderBin("v_textured.vert"),
            Data::loadShaderBin("f_textured.frag"),
            true
    );
    Material material("textured",textured);

    if (pScene) {
        std::cout << "init mesh, size : " << pScene->mNumMeshes << std::endl;
        std::cout << "reading mesh as scene : " << fileName.c_str() << std::endl;

        for (int i = 0; i < pScene->mNumMeshes; ++i) {
            const aiMesh *paiMesh = pScene->mMeshes[i];
            if(paiMesh->mPrimitiveTypes!= aiPrimitiveType_TRIANGLE){
                continue;
            }
            std::string name = fileName + std::to_string(i);
            std::cout<<name<<std::endl;

            auto obj = new Object(name);
            obj->setParent(root);

            auto transform = obj->addComponent<CTransform>();
            auto rend = obj->addComponent<CMeshRenderer>();

            auto mesh = std::make_shared<BaseMesh>();
            mesh->subMeshes.resize(1);

            mesh->initMeshAsSingle(paiMesh);
            rend->setMesh(mesh,STATIC, false);
            auto matIdx = paiMesh->mMaterialIndex;
            auto mat = pScene->mMaterials[matIdx];
            auto matInst = material.createInstance();
            rend->setMaterial(matInst);


            std::cout<< pScene->mMaterials[matIdx]->GetName().C_Str()<<std::endl;
            aiString textureName;//Filename of the texture using the aiString assimp structure
            if(mat->GetTextureCount(aiTextureType_DIFFUSE)){
                //Get the file name of the texture by passing the variable by reference again
                //Second param is 0, which is the first diffuse texture
                //There can be more diffuse textures but for now we are only interested in the first one
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
                std::string path = "data/"+std::string(textureName.data);
                std::string textureType = "diffuse ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                std::cout <<textureFileName<<std::endl;
                matInst->setTexture(Data::loadTexture(path.c_str(),Texture::TYPE::COLOR));
            } else {
                matInst->setTexture(Data::loadTexture("data/color_flat.png",Texture::TYPE::COLOR));
            }

            if(mat->GetTextureCount(aiTextureType_HEIGHT)){
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), textureName);
                std::string path = "data/"+std::string(textureName.data);
                std::string textureType = "normal ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                std::cout <<textureFileName<<std::endl;
                matInst->setTexture(Data::loadTexture(path.c_str(),Texture::TYPE::NORMAL));
            } else {
                matInst->setTexture(Data::loadTexture("data/normal_flat.png",Texture::TYPE::NORMAL));
            }

            if(mat->GetTextureCount(aiTextureType_SPECULAR)){
                auto ret = mat->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), textureName);
                std::string path = "data/"+std::string(textureName.data);
                std::string textureType = "specular ";
                std::string textureFileName = textureType + textureName.data;//The actual name of the texture file
                std::cout <<textureFileName<<std::endl;
                matInst->setTexture(Data::loadTexture(path.c_str(),Texture::TYPE::SPECULAR));
            } else {
                matInst->setTexture(Texture::TYPE::SPECULAR,Data::loadTexture("data/color_flat.png",Texture::TYPE::COLOR));
            }


            // material index is the ID of the material you are interested in
            float shininess;
            if(AI_SUCCESS != aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess))
            {
                shininess = 20.f;
            }
            else {
                matInst->setSpecColor({1.0,1.0,1.0});
                matInst->setSpecularStr(shininess);
            }



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
    std::string cat = std::string(vertex)+std::string(fragment);
    if (programCache.contains(cat)) {
        return programCache[cat];
    } else {
        auto created =  bgfx::createProgram(
                Data::loadShaderBin(vertex),
                Data::loadShaderBin(fragment),
                true
        );
        programCache[cat] = created;
        return created;
    }
}
