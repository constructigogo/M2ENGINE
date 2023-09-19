//
// Created by Constantin on 20/07/2023.
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H
#include "../Singleton.h"
#include "../Components/CMeshRenderer.h"
#include "../Components/CTransform.h"
#include "Texture.h"
#include "Mesh.h"

using namespace Engine;

namespace Engine {
    class CMeshRenderer;

    class Renderer{
    public:
        Renderer();
        void render();
        static void registerAsDynamic(CMeshRenderer*);
        static void registerAsStatic(CMeshRenderer*);
        static void unregisterDynamic(CMeshRenderer*);
        static void unregisterStatic(CMeshRenderer*);

    private:
        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::pair<std::vector<std::shared_ptr<Texture>>,std::vector<CTransform*>>>> instancing;
        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::pair<bgfx::InstanceDataBuffer, int>>> staticInstanceCache;
        static std::vector<CMeshRenderer*> renderList;
        bgfx::UniformHandle s_texColor;
        bgfx::UniformHandle s_texNormal;
    };
}


#endif //ENGINE_RENDERER_H
