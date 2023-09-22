//
// Created by Constantin on 20/07/2023.
//

#include <iostream>
#include "Renderer.h"
#include "../Core/Data.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<std::shared_ptr<Texture>>, std::vector<CTransform *>>>> Renderer::instancing;
std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> Renderer::staticInstanceCache;
std::vector<CMeshRenderer *> Renderer::renderList;


void Renderer::render() {
    const bgfx::Caps *caps = bgfx::getCaps();

    bool m_shadowSamplerSupported = 0 != (caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL);
    bool m_useShadowSampler = m_shadowSamplerSupported;


    assert(m_useShadowSampler);
    assert(bgfx::isValid(m_shadowMapFB));

    // Define matrices.
    shadowmap_size=2048;
    float shadowTexel[4]{
        1.0f/(float)shadowmap_size,1.0f/(float)shadowmap_size,0.0,0.0
    };

    float lightPos[4]{
            -1.0, 7.0, 1.0, 0.0
    };
    bgfx::setUniform(u_lightPos, lightPos);

    const float area = 32.0f;
    glm::mat4x4 lightViewGLM = glm::lookAt(glm::vec3{lightPos[0], lightPos[1], lightPos[2]}, {0.0, 0.0, 0.0},
                                           {0.0, 1.0, 0.0});
    glm::mat4x4 lightProj = glm::orthoRH_NO(-area, area, -area, area, -50.0f, 100.0f);


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


    bgfx::setViewTransform(RENDER_PASS::Shadow, glm::value_ptr(lightViewGLM), glm::value_ptr(lightProj));
    bgfx::setViewFrameBuffer(RENDER_PASS::Shadow, m_shadowMapFB);
    bgfx::setViewRect(RENDER_PASS::Shadow, 0, 0, shadowmap_size, shadowmap_size);

    glm::mat4x4 viewGLM = glm::lookAt(glm::vec3{0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});
    glm::mat4x4 projGLM = glm::perspective(glm::radians(75.0f), float(width) / float(height), 0.1f, 250.0f);

    bgfx::setViewTransform(RENDER_PASS::Render, view, glm::value_ptr(projGLM));
    bgfx::setViewRect(RENDER_PASS::Render, 0, 0, uint16_t(width), uint16_t(height));


    for (const auto &mesh: renderList) {

        if (!mesh->isActive() || !mesh->getObject()->isActive() || !mesh->mesh) {
            continue;
        }

        const bx::Vec3 &pos = mesh->transform->getPosition();
        const bx::Vec3 &scale = mesh->transform->getScale();
        const bx::Quaternion &rot = mesh->transform->getRotation();
        //std::cout<< pos.x <<" "<<pos.y <<" "<<pos.z <<" " <<std::endl;
        float mtxQuat[16];
        bx::mtxFromQuaternion(mtxQuat, rot);
        mtxQuat[12] = pos.x;
        mtxQuat[13] = pos.y;
        mtxQuat[14] = pos.z;
        float mtxPos[16];
        bx::mtxScale(mtxPos, scale.x, scale.y, scale.z);
        float mtx[16];
        bx::mtxMul(mtx, mtxQuat, mtxPos);

        ///Shadow pass
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, mesh->mesh->VBH);
        bgfx::setIndexBuffer(mesh->mesh->IBH);
        bx::mtxMul(lightMtx, mtx, mtxShadow);

        bgfx::setState(0 | BGFX_STATE_WRITE_RGB
                       | BGFX_STATE_WRITE_Z
                       | BGFX_STATE_DEPTH_TEST_LESS
                       | BGFX_STATE_CULL_CCW
                       | BGFX_STATE_MSAA);
        bgfx::submit(RENDER_PASS::Shadow, shadowmapShader, 0);


        ///Render pass
        //Texture binding
        if (!mesh->textures.empty()) {
            if (bgfx::isValid(mesh->textures[0]->getHandle())) {
                bgfx::setTexture(0, s_texColor, mesh->textures[0]->getHandle());
            }
            if (bgfx::isValid(mesh->textures[1]->getHandle())) {
                bgfx::setTexture(1, s_texNormal, mesh->textures[1]->getHandle());
            }
        }
        bgfx::setTexture(3, s_shadowMap, shadowMapTexture);

        //Uniform ginding
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, mesh->mesh->VBH);
        bgfx::setIndexBuffer(mesh->mesh->IBH);
        glm::mat4x4 lightMtxGLM= lightProj * lightViewGLM;
        bgfx::setUniform(u_shadowTexelSize,shadowTexel);
        bgfx::setUniform(u_lightMtx, mtxShadow);
        bgfx::setUniform(u_lightPos, lightPos);
        bgfx::setState(0
                       | BGFX_STATE_WRITE_RGB
                       | BGFX_STATE_WRITE_A
                       | BGFX_STATE_WRITE_Z
                       | BGFX_STATE_DEPTH_TEST_LESS
                       | BGFX_STATE_CULL_CW
                       | BGFX_STATE_MSAA);
        if (bgfx::isValid(mesh->material)) {
            bgfx::submit(RENDER_PASS::Render, mesh->material, 0);
        } else {
            bgfx::submit(RENDER_PASS::Render, debugShader, 0);
        }
    }

    const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);
    if (!instancingSupported) {
        return;
    }
    const uint16_t instanceStride = 64;
    for (const auto &batch: instancing) {
        const auto mesh = batch.first;
        const auto &batchData = batch.second;
        const auto &transformList = batchData.second.second;
        const auto &texturesList = batchData.second.first;
        uint32_t drawnInst = bgfx::getAvailInstanceDataBuffer(transformList.size(), instanceStride);

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, drawnInst, instanceStride);
        uint8_t *data = idb.data;

#pragma omp parallel for default(none) shared(transformList, data)
        for (int i = 0; i < transformList.size(); ++i) {
            int dataIdx = i * instanceStride;
            const auto ptr = transformList[i];
            const auto &pos = ptr->getPosition();
            const auto &rot = ptr->getRotation();
            const auto &scale = ptr->getScale();

            float mtxQuat[16];
            bx::mtxFromQuaternion(mtxQuat, rot);
            mtxQuat[12] = pos.x;
            mtxQuat[13] = pos.y;
            mtxQuat[14] = pos.z;
            float mtxScale[16];
            bx::mtxScale(mtxScale, scale.x, scale.y, scale.z);


            auto *mtx = (float *) (data + i * instanceStride);
            //bx::mtxTranslate(mtx, pos.x, pos.y, pos.z);
            bx::mtxMul(mtx, mtxQuat, mtxScale);

        }
        bgfx::setVertexBuffer(0, mesh->VBH);
        bgfx::setIndexBuffer(mesh->IBH);
        bgfx::setInstanceDataBuffer(&idb);

        if (!texturesList.empty()) {
            if (bgfx::isValid(texturesList[0]->getHandle())) {
                bgfx::setTexture(0, s_texColor, texturesList[0]->getHandle());
            }
            if (bgfx::isValid(texturesList[1]->getHandle())) {
                bgfx::setTexture(1, s_texNormal, texturesList[1]->getHandle());
            }
        }


        bgfx::setState(0
                       | BGFX_STATE_WRITE_RGB
                       | BGFX_STATE_WRITE_A
                       | BGFX_STATE_WRITE_Z
                       | BGFX_STATE_DEPTH_TEST_LESS
                       | BGFX_STATE_CULL_CCW
                       | BGFX_STATE_MSAA);
        bgfx::submit(0, *batchData.first);

    }

    bgfx::touch(RENDER_PASS::Render);
    bgfx::touch(RENDER_PASS::Shadow);
    bgfx::frame();
}


void Renderer::registerAsDynamic(CMeshRenderer *cMesh) {
    std::cout << "registering dynamic " << cMesh->material.idx << std::endl;
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

void Renderer::setView(float *view) {
    Renderer::view = view;
}

void Renderer::setProj(float *proj) {
    Renderer::proj = proj;
}

void Renderer::init() {
    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
    s_shadowMap = bgfx::createUniform("s_shadowMap", bgfx::UniformType::Sampler);
    u_lightPos = bgfx::createUniform("u_lightPos", bgfx::UniformType::Vec4);
    u_lightMtx = bgfx::createUniform("u_lightMtx", bgfx::UniformType::Mat4);
    u_shadowTexelSize = bgfx::createUniform("u_shadowTexelSize",bgfx::UniformType::Vec4);


    bgfx::setViewClear(RENDER_PASS::Shadow, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewClear(RENDER_PASS::Render, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewMode(RENDER_PASS::Render, bgfx::ViewMode::Default);
    bgfx::setViewMode(RENDER_PASS::Shadow, bgfx::ViewMode::Default);


    debugShader = bgfx::createProgram(
            Data::loadShaderBin("v_simple.vert"),
            Data::loadShaderBin("f_simple.frag"),
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


}
