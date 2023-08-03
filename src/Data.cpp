//
// Created by Constantin on 23/07/2023.
//

#include <fstream>
#include <iostream>
#include "Data.h"

using namespace Engine;

std::unordered_map<std::string, std::shared_ptr<BaseMesh>> Engine::Data::loadedMeshes;

std::shared_ptr<BaseMesh> Engine::Data::loadMesh(const std::string& fileName) {
    auto it = loadedMeshes.find(fileName);
    if (it != loadedMeshes.end()) {
        return loadedMeshes[fileName];
    } else {
        auto created = std::make_shared<BaseMesh>();
        created->loadMesh(fileName);
        loadedMeshes[fileName]= created;
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
    delete [] data;
    return handle;
}