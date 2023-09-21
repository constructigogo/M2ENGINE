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


        Renderer(int width, int height);

        void init();

        void render();

        void setRect(int width, int height);

        void setView(float *view);

        void setProj(float *proj);

        static void registerAsDynamic(CMeshRenderer*);
        static void registerAsStatic(CMeshRenderer*);
        static void unregisterDynamic(CMeshRenderer*);
        static void unregisterStatic(CMeshRenderer*);

    private:

        enum RENDER_PASS{
            Shadow,
            Render,
            Count
        };

        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::pair<std::vector<std::shared_ptr<Texture>>,std::vector<CTransform*>>>> instancing;
        static std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::pair<bgfx::InstanceDataBuffer, int>>> staticInstanceCache;
        static std::vector<CMeshRenderer*> renderList;

        bgfx::FrameBufferHandle m_shadowMapFB = BGFX_INVALID_HANDLE;

        bgfx::TextureHandle shadowMapTexture;

        bgfx::UniformHandle s_texColor;
        bgfx::UniformHandle s_texNormal;
        bgfx::UniformHandle s_shadowMap;
        bgfx::UniformHandle u_lightPos;
        bgfx::UniformHandle u_lightMtx;
        bgfx::UniformHandle u_shadowTexelSize;
        bgfx::UniformHandle u_depthScaleOffset;


        bgfx::ProgramHandle debugShader;
        bgfx::ProgramHandle shadowmapShader;

        float* view;
        float* proj;
        int width;

        int height;


        int shadowmap_size=1024;
    };
}


#endif //ENGINE_RENDERER_H
