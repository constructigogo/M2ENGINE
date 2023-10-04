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
#include "debugdraw/debugdraw.h"

using namespace Engine;

namespace Engine {
    class CMeshRenderer;

    class Renderer {
    public:
        Renderer();


        Renderer(int width, int height);

        void init();

        void render();

        void setRect(int width, int height);

        void setView(glm::mat4x4 view);

        void setProj(float *proj);

        void toggleDrawDebugShape();

        static void registerAsDynamic(CMeshRenderer *);

        static void registerAsStatic(CMeshRenderer *);

        static void unregisterDynamic(CMeshRenderer *);

        static void unregisterStatic(CMeshRenderer *);

    private:

        enum RENDER_PASS {
            Shadow,
            //Geometry, UNUSED
            ZPREPASS,
            Render,
            DEBUG,
            SSAO,
            SSS,
            Count
        };

        static std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<std::shared_ptr<Texture>>, std::vector<CTransform *>>>> instancing;
        static std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> staticInstanceCache;
        static std::vector<CMeshRenderer *> renderList;

        bgfx::FrameBufferHandle m_shadowMapFB = BGFX_INVALID_HANDLE;

        bgfx::TextureHandle shadowMapTexture;

        bgfx::UniformHandle s_texColor;
        bgfx::UniformHandle s_texNormal;
        bgfx::UniformHandle s_texSpecular;
        bgfx::UniformHandle s_shadowMap;
        bgfx::UniformHandle u_specular;
        bgfx::UniformHandle u_lightPos;
        bgfx::UniformHandle u_lightMtx;
        bgfx::UniformHandle u_shadowTexelSize;
        bgfx::UniformHandle u_depthScaleOffset;


        bgfx::ProgramHandle debugShader;
        bgfx::ProgramHandle shadowmapShader;

        glm::mat4x4 view;
        glm::mat4x4 proj;
        int width;
        int height;
        bool drawDebugShapes= false;


        int shadowmap_size = 1024;
    };

    inline bool isCulled(const glm::mat4x4 &VP, const glm::mat4x4 &M, const Box &bbox) {

        const auto & verts = bbox.getVertices();
        std::array<glm::vec4,8> tv{};
        for (int i = 0; i < 8; ++i) {
            tv[i] = VP*M*glm::vec4(verts[i],1.0f);
        }
        bool left = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.x < -v.w;});
        bool right = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.x > v.w;});
        bool up = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.y < -v.w;});
        bool down = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.y > v.w;});
        bool front = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.z < -v.w;});
        bool far = std::all_of(tv.begin(), tv.end(),[](const glm::vec4& v){return v.z > v.w;});
        bool isAllOutside = left || right || up || down || front || far;

        return isAllOutside;
    }


}


#endif //ENGINE_RENDERER_H
