//
// Created by Constantin on 20/07/2023.
//

#ifndef ENGINE_RENDERER_H
#define ENGINE_RENDERER_H
#include "Singleton.h"
#include "Components/CMeshRenderer.h"
#include "Components/CTransform.h"
#include "Mesh.h"

using namespace Engine;

namespace Engine {
    class CMeshRenderer;

    class Renderer{
    public:
        Renderer()= default;
        void render();
        static void registerAsDynamic(CMeshRenderer*);
        static void registerAsStatic(CMeshRenderer*);
        static void unregisterDynamic(CMeshRenderer*);
        static void unregisterStatic(CMeshRenderer*);

    private:
        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::vector<CTransform*>>> instancing;
        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::pair<bgfx::InstanceDataBuffer, int>>> staticInstanceCache;
        static std::vector<CMeshRenderer*> renderList;
    };
}


#endif //ENGINE_RENDERER_H
