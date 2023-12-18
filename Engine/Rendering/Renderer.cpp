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
#include "../Physic/PBDContainer.h"

std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<std::shared_ptr<Texture>>, std::vector<CTransform *>>>> Renderer::instancing;
std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> Renderer::staticInstanceCache;
std::vector<CMeshRenderer *> Renderer::renderList;
std::vector<CParticleContainer *> Renderer::emittersList;


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
    static bgfx::VertexLayout ms_DataLayout;
    static bgfx::VertexLayout ms_instance_layout;
    if (!cache && renderList.size() > 0) {
        auto res = Data::buildIndirectCache();
        VBH = res.first;
        IBH = res.second;
        ms_layout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .end();

        ms_DataLayout.begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
                .end();

        ms_instance_layout
                .begin()
                .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord1, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord2, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord3, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord4, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord5, 4, bgfx::AttribType::Float)
                .end();
        cache = true;
    }

    for (auto & Cparticle: emittersList) {
        auto & cont = Cparticle->getContainer();
        auto data = cont->data();
        if(!data.empty()&&Cparticle->isActive()) {
            const bgfx::Memory *datamem = bgfx::alloc(sizeof(ParticleData) * data.size());
            ParticleData *objData = (ParticleData *) datamem->data;
            int counter = 0;
            for (auto pos: data) {
                objData[counter].position = pos;
                counter++;
            }


            if (bgfx::isValid(particle_buffer)) {
                bgfx::destroy(particle_buffer);
            }
            //instance_buffer = bgfx::createDynamicVertexBuffer(renderList.size(), ms_instance_layout,BGFX_BUFFER_COMPUTE_WRITE);
            particle_buffer = bgfx::createDynamicVertexBuffer(datamem, ms_particle_layout, BGFX_BUFFER_NONE);
            //particle_buffer = bgfx::createDynamicVertexBuffer(bgfx::copy(data.data(),data.size()*sizeof(glm::vec3)), ms_particle_layout, BGFX_BUFFER_NONE);
            bgfx::setVertexBuffer(0, particle_buffer);
            bgfx::setState(0
                           | BGFX_STATE_WRITE_RGB
                           | BGFX_STATE_WRITE_A
                           | BGFX_STATE_WRITE_Z
                           | BGFX_STATE_DEPTH_TEST_LESS
                           | BGFX_STATE_PT_POINTS
                           | BGFX_STATE_POINT_SIZE(15));
            bgfx::submit(RENDER_PASS::Render, particle_program);
        }
    }


    std::vector<glm::mat4> models(renderList.size());
    std::vector<InstanceDrawData> instData;
    //std::vector<MultiDrawData> multData;

    if (!renderList.empty()) {
        const bgfx::Memory *mem = bgfx::alloc(sizeof(InstanceDrawData) * renderList.size());
        InstanceDrawData *objs = (InstanceDrawData *) mem->data;

        const bgfx::Memory *datamem = bgfx::alloc(sizeof(InstanceObjectData) * renderList.size());
        InstanceObjectData *objData = (InstanceObjectData *) datamem->data;

        int counter = 0;
        for (const auto &mesh: renderList) {
            if (!mesh->getObject()->isActive()) continue;
            glm::mat4 mtx = mesh->transform->getTransformMTX();
            models.push_back(mtx);
            objs[counter].vertexOffset = mesh->mesh->vOffsetInGlobal;
            objs[counter].vertexCount = mesh->mesh->vertexesAllData.size(); // m_vertexCount is unused in compute shader, its only here for completeness
            objs[counter].indexOffset = mesh->mesh->IOffsetInGlobal;
            objs[counter].indexCount = mesh->mesh->indicesAllData.size();

            objData[counter].transform = mesh->transform->getTransformMTX();
            objData[counter].bmin = glm::vec4(mesh->getBBox().getLower(), 0);
            objData[counter].bmax = glm::vec4(mesh->getBBox().getUpper(), 0);
            counter++;

            if (drawDebugShapes) {
                auto lower = mesh->getBBox().getLower();
                auto upper = mesh->getBBox().getUpper();
                enc.push();
                enc.pushTransform(glm::value_ptr(mtx));
                bx::Aabb aabb =
                        {
                                {lower.x, lower.y, lower.z},
                                {upper.x, upper.y, upper.z},
                        };
                enc.setWireframe(true);
                enc.setColor(0xFF0000FF);
                enc.draw(aabb);
                enc.popTransform();
                enc.pop();
            }
        }

        if (bgfx::isValid(object_list_buffer)) {
            bgfx::destroy(object_list_buffer);
        }
        object_list_buffer = bgfx::createVertexBuffer(mem, ms_layout, BGFX_BUFFER_COMPUTE_READ);

        if (bgfx::isValid(instance_buffer)) {
            bgfx::destroy(instance_buffer);
        }
        //instance_buffer = bgfx::createDynamicVertexBuffer(renderList.size(), ms_instance_layout,BGFX_BUFFER_COMPUTE_WRITE);
        instance_buffer = bgfx::createDynamicVertexBuffer(datamem, ms_instance_layout, BGFX_BUFFER_COMPUTE_READ);
        if (bgfx::isValid(indirect_buffer_handle)) {
            bgfx::destroy(indirect_buffer_handle);
        }
        indirect_buffer_handle = bgfx::createIndirectBuffer(renderList.size());


        if (bgfx::isValid(indirect_count_buffer_handle)) {
            bgfx::destroy(indirect_count_buffer_handle);
        }
        const bgfx::Memory *countMem = bgfx::alloc(sizeof(uint32_t));
        *(uint32_t *) countMem->data = 0;
        indirect_count_buffer_handle = bgfx::createIndexBuffer(countMem,
                                                               BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_WRITE |
                                                               BGFX_BUFFER_DRAW_INDIRECT);

        if (true) {///ShadowPass
            bgfx::setBuffer(0, object_list_buffer, bgfx::Access::Read);
            bgfx::setBuffer(1, indirect_buffer_handle, bgfx::Access::Write);
            bgfx::setBuffer(2, instance_buffer, bgfx::Access::Read);
            bgfx::setBuffer(3, indirect_count_buffer_handle, bgfx::Access::Write);
            glm::vec4 fmin = glm::vec4(bbox.getLower(), 1.0);
            glm::vec4 fmax = glm::vec4(bbox.getUpper(), 1.0);
            glm::vec4 numtodraw = glm::vec4(counter, 0, 0, 0);
            bgfx::setUniform(u_fmin, glm::value_ptr(fmin));
            bgfx::setUniform(u_fmax, glm::value_ptr(fmax));
            bgfx::setUniform(u_numToDraw, glm::value_ptr(numtodraw));

            bgfx::dispatch(RENDER_PASS::Shadow, indirect_count_program, uint32_t(counter / 64 + 1), 1, 1);

            bgfx::setVertexBuffer(0, VBH);
            bgfx::setIndexBuffer(IBH);
            bgfx::setInstanceDataBuffer(instance_buffer, 0, renderList.size());

            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(RENDER_PASS::Shadow, shadowmapShader, indirect_buffer_handle, 0,
                         indirect_count_buffer_handle);
        }


        bgfx::setBuffer(0, object_list_buffer, bgfx::Access::Read);
        bgfx::setBuffer(1, indirect_buffer_handle, bgfx::Access::Write);
        bgfx::setBuffer(2, instance_buffer, bgfx::Access::Read);
        bgfx::setBuffer(3, indirect_count_buffer_handle, bgfx::Access::Write);
        glm::vec4 fmin = glm::vec4(bbox.getLower(), 1.0);
        glm::vec4 fmax = glm::vec4(bbox.getUpper(), 1.0);
        glm::vec4 numtodraw = glm::vec4(counter, 0, 0, 0);
        bgfx::setUniform(u_fmin, glm::value_ptr(fmin));
        bgfx::setUniform(u_fmax, glm::value_ptr(fmax));
        bgfx::setUniform(u_numToDraw, glm::value_ptr(numtodraw));

        bgfx::dispatch(RENDER_PASS::Render, indirect_count_program, uint32_t(counter / 64 + 1), 1, 1);

        bgfx::setVertexBuffer(0, VBH);
        bgfx::setIndexBuffer(IBH);
        bgfx::setInstanceDataBuffer(instance_buffer, 0, renderList.size());

        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(RENDER_PASS::Render, debugInstancedShader, indirect_buffer_handle, 0,
                     indirect_count_buffer_handle);
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
    u_fmin = bgfx::createUniform("u_fmin", bgfx::UniformType::Vec4);
    u_fmax = bgfx::createUniform("u_fmax", bgfx::UniformType::Vec4);
    u_numToDraw = bgfx::createUniform("u_numToDraw", bgfx::UniformType::Vec4);
    u_specular = bgfx::createUniform("u_specular", bgfx::UniformType::Vec4);
    u_lightPos = bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
    u_lightMtx = bgfx::createUniform("u_lightMtx", bgfx::UniformType::Mat4);
    u_shadowTexelSize = bgfx::createUniform("u_shadowTexelSize", bgfx::UniformType::Vec4);
    u_particle_draw = bgfx::createUniform("u_particle_draw", bgfx::UniformType::Vec4);

    ms_particle_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();

    bgfx::setViewClear(RENDER_PASS::Shadow, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewClear(RENDER_PASS::Render, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewClear(RENDER_PASS::DEBUG, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
    bgfx::setViewMode(RENDER_PASS::Render, bgfx::ViewMode::Default);
    bgfx::setViewMode(RENDER_PASS::Shadow, bgfx::ViewMode::Default);
    bgfx::setViewMode(RENDER_PASS::DEBUG, bgfx::ViewMode::Default);


    indirect_program = bgfx::createProgram(Data::loadShaderBin("cs_indirect.compute"), true);
    indirect_count_program = bgfx::createProgram(Data::loadShaderBin("cs_indirect_count.compute"), true);
    indirect_culling_program = bgfx::createProgram(Data::loadShaderBin("cs_culling.compute"), true);
    indirect_buffer_handle = BGFX_INVALID_HANDLE;
    indirect_count_buffer_handle = BGFX_INVALID_HANDLE;
    object_list_buffer = BGFX_INVALID_HANDLE;
    instance_buffer = BGFX_INVALID_HANDLE;
    instance_OutBuffer = BGFX_INVALID_HANDLE;
    particle_buffer = BGFX_INVALID_HANDLE;


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
            Data::loadShaderBin("v_shadowmap_inst.vert"),
            Data::loadShaderBin("f_shadowmap.frag"),
            true
    );

    particle_program = bgfx::createProgram(
            Data::loadShaderBin("v_particle.vert"),
            Data::loadShaderBin("f_particle.frag"),
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

void Renderer::registerEmitter(CParticleContainer * ptr) {
    emittersList.push_back(ptr);
}

void Renderer::unregisterEmitter(CParticleContainer * ptr) {
    emittersList.erase(std::remove(emittersList.begin(), emittersList.end(), ptr), emittersList.end());
}
