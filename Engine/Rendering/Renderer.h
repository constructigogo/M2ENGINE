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
#include "../Core/GenericHandle.h"
#include "../Components/CParticleContainer.h"

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

        static void registerEmitter(CParticleContainer *);

        static void unregisterEmitter(CParticleContainer *);

    private:

        struct InstanceDrawData {
            float vertexOffset;
            float vertexCount;
            float indexOffset;
            float indexCount;
        };

        struct InstanceObjectData {
            glm::mat4x4 transform;
            glm::vec4 bmin;
            glm::vec4 bmax;
        };

        struct ParticleData {
            glm::vec3 position;
        };

        enum RENDER_PASS {
            Shadow,
            //Geometry, UNUSED
            ZPREPASS,
            ZDownscale,
            Occlude,
            Render,
            DEBUG,
            SSAO,
            SSS,
            Count
        };


        void RENDER_DownscaleZ();

        void RENDER_Occluding();


        static std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<std::shared_ptr<Texture>>, std::vector<CTransform *>>>> instancing;
        static std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> staticInstanceCache;
        static std::vector<CMeshRenderer *> renderList;
        static std::vector<CParticleContainer *> emittersList;

        int ZDepthRes = 512;

        bgfx::FrameBufferHandle m_shadowMapFB = BGFX_INVALID_HANDLE;
        bgfx::FrameBufferHandle m_ZDepthBuffer = BGFX_INVALID_HANDLE;
        bgfx::FrameBufferHandle m_ZBuffer = BGFX_INVALID_HANDLE;

        int m_ZMipCount = 0;

        bgfx::TextureHandle shadowMapTexture;
        bgfx::TextureHandle ZPrepassTexture;

        //Samplers
        UniformHandle s_texColor;
        UniformHandle s_texNormal;
        UniformHandle s_texSpecular;
        UniformHandle s_shadowMap;
        UniformHandle s_texOcclusionDepth;

        //Uniforms
        UniformHandle u_fmin;
        UniformHandle u_fmax;
        UniformHandle u_numToDraw;
        UniformHandle u_specular;
        UniformHandle u_lightPos;
        UniformHandle u_lightMtx;
        UniformHandle u_shadowTexelSize;
        UniformHandle u_depthScaleOffset;
        UniformHandle u_particle_draw;
        UniformHandle u_input_Z_RT_size;
        UniformHandle u_cullingConfig;

        ProgramHandle indirect_program;
        ProgramHandle indirect_count_program;
        ProgramHandle indirect_culling_program;
        ProgramHandle copyZ_program;
        ProgramHandle downscaleZ_program;
        ProgramHandle occlude_program;
        ProgramHandle particle_program;


        bgfx::IndirectBufferHandle indirect_buffer_handle;
        bgfx::IndexBufferHandle indirect_count_buffer_handle;
        bgfx::VertexBufferHandle object_list_buffer;
        bgfx::DynamicVertexBufferHandle instance_buffer;
        bgfx::DynamicVertexBufferHandle instance_OutBuffer;
        bgfx::DynamicVertexBufferHandle particle_buffer;
        bgfx::DynamicIndexBufferHandle m_occlusionPredicate;

        bgfx::VertexLayout ms_layout;
        bgfx::VertexLayout ms_DataLayout;
        bgfx::VertexLayout ms_instance_layout;

        bgfx::VertexLayout ms_particle_layout;


        ProgramHandle debugShader;
        ProgramHandle debugInstancedShader;
        ProgramHandle ZPrepassInstancedShader;
        ProgramHandle shadowmapShader;

        glm::mat4x4 view;
        glm::mat4x4 proj;
        int width;
        int height;
        int drawCountThisFrame = 0;

        uint32_t m_hiZwidth;
        uint32_t m_hiZheight;

        bool drawDebugShapes = false;

        bool firstFrame = true;
        int shadowmap_size = 1024;
    };

    inline bool isCulled(const glm::mat4x4 &VP, const glm::mat4x4 &M, const Box &bbox) {

        const auto &verts = bbox.getVertices();
        std::array<glm::vec4, 8> tv{};
        for (int i = 0; i < 8; ++i) {
            tv[i] = VP * M * glm::vec4(verts[i], 1.0f);
        }
        bool left = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.x < -v.w; });
        bool right = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.x > v.w; });
        bool up = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.y < -v.w; });
        bool down = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.y > v.w; });
        bool front = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.z < -v.w; });
        bool far = std::all_of(tv.begin(), tv.end(), [](const glm::vec4 &v) { return v.z > v.w; });
        bool isAllOutside = left || right || up || down || front || far;

        return isAllOutside;
    }


}


#endif //ENGINE_RENDERER_H
