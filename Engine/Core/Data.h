//
// Created by Constantin on 23/07/2023.
//

#ifndef ENGINE_DATA_H
#define ENGINE_DATA_H


#include <unordered_map>
#include <unordered_set>
#include <string>
#include "../Rendering/Mesh.h"
#include "../Rendering/Texture.h"
#include "bx/readerwriter.h"
#include "bx/file.h"
#include <memory>

namespace Engine {
    class Data {
    public:
        static void init();
        static void cleanup();


        static const std::unordered_set<std::string> &getLoadableMesh();
        static std::shared_ptr<BaseMesh> loadMesh(const std::string &fileName, bool simpleImport=false);
        static std::vector<Object *> loadScene(const std::string &fileName);
        static bgfx::ShaderHandle loadShaderBin(const char *_name);
        static bgfx::TextureHandle loadTextureRaw(const char *_name);
        static std::shared_ptr<Texture> loadTexture(const char *_name);
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
        static std::unordered_map<std::string, std::shared_ptr<BaseMesh>> meshCache;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
        static std::unordered_map<std::string, bgfx::TextureHandle> textureCacheRaw;
        static std::unordered_set<std::string> loadableMesh;
    };
}


#endif //ENGINE_DATA_H
