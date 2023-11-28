//
// Created by Constantin on 20/07/2023.
//

#include <iostream>
#include "Renderer.h"
#include "../Core/Data.h"
#include "../Core/Log.h"
#include "../Core/Box.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<std::shared_ptr<Texture>>, std::vector<CTransform *>>>> Renderer::instancing;
std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> Renderer::staticInstanceCache;
std::vector<CMeshRenderer *> Renderer::renderList;


void Renderer::render() {
    const bgfx::Caps *caps = bgfx::getCaps();

    const bool computeSupported = !!(BGFX_CAPS_COMPUTE & bgfx::getCaps()->supported);
    const bool indirectSupported = !!(BGFX_CAPS_DRAW_INDIRECT & bgfx::getCaps()->supported);

    const bool m_shadowSamplerSupported = 0 != (caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL);
    bool m_useShadowSampler = m_shadowSamplerSupported;

    assert(m_useShadowSampler);
    assert(bgfx::isValid(m_shadowMapFB));

    // Define matrices.qe
    shadowmap_size = 2048;
    float shadowTexel[4]{
            1.0f / (float) shadowmap_size, 1.0f / (float) shadowmap_size, 0.0, 0.0
    };

    float lightPos[4]{
            -1.0, 7.0, 1.0, 0.0
    };
    bgfx::setUniform(u_lightPos, lightPos);

    //Light view mtx
    const float area = 32.0f;
    glm::mat4x4 lightViewGLM = glm::lookAt(glm::vec3{lightPos[0], lightPos[1], lightPos[2]}, {0.0, 0.0, 0.0},
                                           {0.0, 1.0, 0.0});
    glm::mat4x4 lightProj = glm::orthoRH_NO(-area, area, -area, area, -100.0f, 30.0f);


    float mtxShadow[16];
    float lightMtx[16];

    const float sy = caps->originBottomLeft ? 0.5f : -0.5f;
    const float sz = caps->homogeneousDepth ? 0.5f : 1.0f;
    const float tz = caps->homogeneousDepth ? 0.5f : 0.0f;
    const float mtxCrop[16] =
            {
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, sy, 0.0f, 0.0f,
                    0.0f, 0.0f, sz, 0.0f,
                    0.5f, 0.5f, tz, 1.0f,
            };

    float mtxTmp[16];
    bx::mtxMul(mtxTmp, glm::value_ptr(lightProj), mtxCrop);
    bx::mtxMul(mtxShadow, glm::value_ptr(lightViewGLM), mtxTmp);

    //Shadow Settings
    bgfx::setViewTransform(RENDER_PASS::Shadow, glm::value_ptr(lightViewGLM), glm::value_ptr(lightProj));
    bgfx::setViewFrameBuffer(RENDER_PASS::Shadow, m_shadowMapFB);
    bgfx::setViewRect(RENDER_PASS::Shadow, 0, 0, shadowmap_size, shadowmap_size);

    //Render mtx
    float near = 0.1f;
    float far = 250.0f;
    glm::mat4x4 viewGLM = glm::lookAt(glm::vec3{0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});
    glm::mat4x4 projGLM = glm::perspective(glm::radians(75.0f), float(width) / float(height), near, far);

    //Render Frustum
    Box bbox({0.0, 0.0, near}, {1.0, 1.0, -far});

    bgfx::setViewTransform(RENDER_PASS::Render, glm::value_ptr(view), glm::value_ptr(projGLM));
    bgfx::setViewRect(RENDER_PASS::Render, 0, 0, uint16_t(width), uint16_t(height));

    DebugDrawEncoder enc;
    enc.begin(RENDER_PASS::Render, true, bgfx::begin());


    static bool cache = false;
    static bgfx::VertexBufferHandle VBH;
    static bgfx::IndexBufferHandle IBH;
    static bgfx::VertexLayout ms_layout;
    static bgfx::VertexLayout ms_instance_layout;
    if (!cache && renderList.size() > 0) {
        auto res = Data::buildIndirectCache();
        VBH = res.first;
        IBH = res.second;
        ms_layout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .end();


        ms_instance_layout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord2, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord3, 4, bgfx::AttribType::Float)
                //.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .end();

        cache = true;
    }

    std::vector<glm::mat4> models(renderList.size());
    std::vector<InstanceDrawData> instData;
    std::vector<MultiDrawData> multData;

    if (!renderList.empty()) {
        const bgfx::Memory *mem = bgfx::alloc(sizeof(InstanceDrawData) * renderList.size());
        InstanceDrawData *objs = (InstanceDrawData *) mem->data;

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, renderList.size(), 64);
        uint8_t* data = idb.data;

        int counter = 0;
        for (const auto &mesh: renderList) {
            glm::mat4 mtx = mesh->transform->getTransformMTX();
            models.push_back(mtx);
            objs[counter].vertexOffset = mesh->mesh->vOffsetInGlobal;
            objs[counter].vertexCount = mesh->mesh->vertexesAllData.size(); // m_vertexCount is unused in compute shader, its only here for completeness
            objs[counter].indexOffset = mesh->mesh->IOffsetInGlobal;
            objs[counter].indexCount = mesh->mesh->indicesAllData.size();
            counter++;
            float* Mmtx = (float*)data;
            for (int i = 0; i < 16; ++i) {
                Mmtx[i]=0.0;
            }
            Mmtx[12]=0.0;
            Mmtx[13]=0.0;
            Mmtx[14]=0.0;
            Mmtx[0]=1.0;
            Mmtx[5]=1.0;
            Mmtx[10]=1.0;
            Mmtx[15]=1.0;

            //*Mmtx=*glm::value_ptr(mtx);
            data+=64;
        }

        if (bgfx::isValid(object_list_buffer)){
            bgfx::destroy(object_list_buffer);
        }
        object_list_buffer = bgfx::createVertexBuffer(mem, ms_layout, BGFX_BUFFER_COMPUTE_READ);
        instance_buffer = bgfx::createDynamicVertexBuffer(renderList.size(), ms_instance_layout, BGFX_BUFFER_COMPUTE_WRITE);
        if (bgfx::isValid(indirect_buffer_handle)){
            bgfx::destroy(indirect_buffer_handle);
        }
        indirect_buffer_handle = bgfx::createIndirectBuffer(renderList.size());

        bgfx::setBuffer(0, object_list_buffer, bgfx::Access::Read);
        bgfx::setBuffer(1, indirect_buffer_handle, bgfx::Access::Write);
        bgfx::setBuffer(2, instance_buffer, bgfx::Access::Write);
        bgfx::dispatch(RENDER_PASS::Render, indirect_program, uint32_t(renderList.size()/64 + 1), 1, 1);

        bgfx::setVertexBuffer(0, VBH);
        bgfx::setIndexBuffer(IBH);
        bgfx::setInstanceDataBuffer(instance_buffer,0,renderList.size());

        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(RENDER_PASS::Render,debugInstancedShader,indirect_buffer_handle,0,renderList.size());
    }

    enc.end();


    bgfx::touch(RENDER_PASS::Render);
    bgfx::touch(RENDER_PASS::Shadow);
    bgfx::frame();
}


void Renderer::registerAsDynamic(CMeshRenderer *cMesh) {
    ENGINE_TRACE("Registering [" + cMesh->getObject()->getName() + "] as dynamic");
    Renderer::renderList.push_back(cMesh);
}

void Renderer::registerAsStatic(CMeshRenderer *cMesh) {

    //bgfx::setInstanceCount()
    const auto key = cMesh->mesh.get();
    if (instancing.contains(cMesh->mesh.get())) {
        std::cout << "registering add" << std::endl;
        instancing[key].second.second.push_back(cMesh->getObject()->getComponent<CTransform>());
    } else {
        std::cout << "registering new " << cMesh->mesh->getName() << cMesh->material.idx << std::endl;
        instancing[key].first = &cMesh->material;
        instancing[key].second.second.push_back(cMesh->getObject()->getComponent<CTransform>());
        for (const auto &texHandle: cMesh->textures) {
            instancing[key].second.first.push_back(texHandle);
        }
    }
    /*
    auto it_static = staticInstanceCache.find(cMesh->mesh.get());
    if (it != instancing.end()) {
        std::cout << "adding instance"<<std::endl;
        //staticInstanceCache[key].second.push_back(cMesh->getParent()->getComponent<CTransform>());
    }
    else {
        std::cout << "new instance"<< cMesh->material.idx<<std::endl;
        staticInstanceCache[key].first = &cMesh->material;
        const uint16_t instanceStride = 64;

        uint32_t drawnInst = bgfx::getAvailInstanceDataBuffer(0, instanceStride);

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, drawnInst, instanceStride);
        //staticInstanceCache[key].second.push_back(cMesh->getParent()->getComponent<CTransform>());
    }
     */
}

void Renderer::unregisterDynamic(CMeshRenderer *cMesh) {
    renderList.erase(std::remove(renderList.begin(), renderList.end(), cMesh), renderList.end());
}

void Renderer::unregisterStatic(CMeshRenderer *cMesh) {
    const auto key = cMesh->mesh.get();
    auto it = instancing.find(key);
    if (it != instancing.end()) {
        const auto tf = cMesh->getObject()->getComponent<CTransform>();
        auto &list = instancing[key].second.second;
        list.erase(std::remove(list.begin(), list.end(), tf), list.end());
    }
}

Renderer::Renderer() {
    init();
}

Renderer::Renderer(int width, int height) : width(width), height(height) {
    init();
}

void Renderer::setRect(int width, int height) {
    this->width = width;
    this->height = height;
    bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(RENDER_PASS::Render, 0, 0, bgfx::BackbufferRatio::Equal);
}

void Renderer::setView(glm::mat4x4 view) {
    Renderer::view = view;
}

void Renderer::setProj(float *proj) {
    //Renderer::proj = proj;
}

void Renderer::init() {
    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
    s_texSpecular = bgfx::createUniform("s_texSpecular", bgfx::UniformType::Sampler);
    s_shadowMap = bgfx::createUniform("s_shadowMap", bgfx::UniformType::Sampler);
    u_specular = bgfx::createUniform("u_specular", bgfx::UniformType::Vec4);
    u_lightPos = bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
    u_lightMtx = bgfx::createUniform("u_lightMtx", bgfx::UniformType::Mat4);
    u_shadowTexelSize = bgfx::createUniform("u_shadowTexelSize", bgfx::UniformType::Vec4);


    bgfx::setViewClear(RENDER_PASS::Shadow, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewClear(RENDER_PASS::Render, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewClear(RENDER_PASS::DEBUG, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
    bgfx::setViewMode(RENDER_PASS::Render, bgfx::ViewMode::Default);
    bgfx::setViewMode(RENDER_PASS::Shadow, bgfx::ViewMode::Default);
    bgfx::setViewMode(RENDER_PASS::DEBUG, bgfx::ViewMode::Default);

    indirect_program=bgfx::createProgram(Data::loadShaderBin("cs_indirect.compute"),true);
    indirect_buffer_handle = BGFX_INVALID_HANDLE;
    indirect_count_buffer_handle = BGFX_INVALID_HANDLE;
    object_list_buffer = BGFX_INVALID_HANDLE;


    debugShader = bgfx::createProgram(
            Data::loadShaderBin("v_simple.vert"),
            Data::loadShaderBin("f_simple.frag"),
            true
    );

    debugInstancedShader = bgfx::createProgram(
            Data::loadShaderBin("v_simple_inst.vert"),
            Data::loadShaderBin("f_simple_inst.frag"),
            true
    );

    shadowmapShader = bgfx::createProgram(
            Data::loadShaderBin("v_shadowmap.vert"),
            Data::loadShaderBin("f_shadowmap.frag"),
            true
    );

    u_depthScaleOffset = bgfx::createUniform("u_depthScaleOffset", bgfx::UniformType::Vec4);

    // Get renderer capabilities info.
    const bgfx::Caps *caps = bgfx::getCaps();

    float depthScaleOffset[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    if (caps->homogeneousDepth) {
        depthScaleOffset[0] = 0.5f;
        depthScaleOffset[1] = 0.5f;
    }
    bgfx::setUniform(u_depthScaleOffset, depthScaleOffset);


    shadowMapTexture = bgfx::createTexture2D(
            shadowmap_size, shadowmap_size, false, 1, bgfx::TextureFormat::D16,
            BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL
    );

    m_shadowMapFB = bgfx::createFrameBuffer((uint8_t) 1, &shadowMapTexture, false);

    ddInit();
}

void Renderer::toggleDrawDebugShape() {
    drawDebugShapes = !drawDebugShapes;
}
