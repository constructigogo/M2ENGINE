//
// Created by Constantin on 23/07/2023.
//

#ifndef ENGINE_DATA_H
#define ENGINE_DATA_H


#include <unordered_map>
#include <string>
#include "Mesh.h"
#include <memory>

namespace Engine {
    class Data {
    public:
        static std::shared_ptr<BaseMesh> loadMesh(const std::string&);
        static bgfx::ShaderHandle loadShaderBin(const char *_name);
    private:
        static std::unordered_map<std::string, std::shared_ptr<BaseMesh>> loadedMeshes;
    };
}


#endif //ENGINE_DATA_H
