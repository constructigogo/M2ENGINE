//
// Created by Constantin on 23/07/2023.
//

#include <fstream>
#include <iostream>
#include "Data.h"
#include "bimg/bimg.h"
#include "bx/readerwriter.h"
#include "bimg/decode.h"


using namespace Engine;

bx::FileReaderI *Data::s_fileReader = nullptr;
bx::AllocatorI *Data::allocator = nullptr;


std::unordered_map<std::string, std::shared_ptr<BaseMesh>> Engine::Data::loadedMeshes;
std::unordered_map<std::string, bgfx::TextureHandle> Engine::Data::textureCache;

std::shared_ptr<BaseMesh> Engine::Data::loadMesh(const std::string &fileName, bool simpleImport) {
    if (loadedMeshes.contains(fileName)) {
        return loadedMeshes[fileName];
    } else {
        auto created = std::make_shared<BaseMesh>();
        created->loadMesh(fileName, simpleImport);
        loadedMeshes[fileName] = created;
        return created;
    }
}

bgfx::ShaderHandle Engine::Data::loadShaderBin(const char *_name) {
    char *data = new char[2048];
    std::string path = "shaders/generated/";
    path.append(_name);
    path.append(".bin");

    std::ifstream file;
    size_t fileSize;
    file.open(path);
    bool success = false;
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        file.read(data, fileSize);
        file.close();
        success = true;
    }
    if (!success) {
        std::cout << "could not load shader " << _name << std::endl;
    }

    const bgfx::Memory *mem = bgfx::copy(data, fileSize + 1);
    mem->data[mem->size - 1] = '\0';
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    bgfx::setName(handle, path.c_str());
    delete[] data;
    return handle;
}

bgfx::TextureHandle Data::loadTexture(const char *_name) {
    if(textureCache.contains(_name)) {
        return textureCache[_name];
    }

    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
    uint64_t _flags=0;
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
    textureCache[_name] = handle;
    return handle;
}

void Data::init() {

    static bx::DefaultAllocator s_allocator;
    allocator = &s_allocator;
    s_fileReader = BX_NEW(allocator, FileReader);
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
    for (auto & [name,texHandle]:textureCache) {
        bgfx::destroy(texHandle);
    }
}
