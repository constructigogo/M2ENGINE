//
// Created by Constantin on 23/07/2023.
//

#ifndef ENGINE_DATA_H
#define ENGINE_DATA_H


#include <unordered_map>
#include <string>
#include "Mesh.h"
#include "bx/readerwriter.h"
#include "bx/file.h"
#include <memory>

namespace Engine {
    class Data {
    public:
        static void init();

        static std::shared_ptr<BaseMesh> loadMesh(const std::string&);
        static bgfx::ShaderHandle loadShaderBin(const char *_name);
        static bgfx::TextureHandle loadTexture(const char *_name);
        static bx::AllocatorI * allocator;
        static bx::FileReaderI* s_fileReader;

        class FileReader : public bx::FileReader
        {
            typedef bx::FileReader super;

        public:
            virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override
            {
                return super::open(_filePath, _err);
            }
        };


    private:
        static void* load(const char *_name, uint32_t* _size);
        static void imageReleaseCb(void* _ptr, void* _userData);
        static std::unordered_map<std::string, std::shared_ptr<BaseMesh>> loadedMeshes;
        static std::unordered_map<std::string, bgfx::TextureHandle> textureCache;
    };
}


#endif //ENGINE_DATA_H
