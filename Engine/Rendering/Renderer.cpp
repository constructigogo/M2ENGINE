//
// Created by Constantin on 20/07/2023.
//

#include <iostream>
#include "Renderer.h"

std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<std::vector<bgfx::TextureHandle>, std::vector<CTransform *>>>> Renderer::instancing;
std::map<BaseMesh *, std::pair<bgfx::ProgramHandle *, std::pair<bgfx::InstanceDataBuffer, int>>> Renderer::staticInstanceCache;
std::vector<CMeshRenderer *> Renderer::renderList;


void Renderer::render() {
    for (const auto mesh: renderList) {

        if(!mesh->isActive() || !mesh->getObject()->isActive() || !mesh->mesh){
            continue;
        }

        const bx::Vec3 &pos = mesh->transform->getPosition();
        const bx::Vec3 &scale = mesh->transform->getScale();
        const bx::Quaternion &rot = mesh->transform->getRotation();

        float mtxQuat[16];
        bx::mtxFromQuaternion(mtxQuat, rot);
        mtxQuat[12] = pos.x;
        mtxQuat[13] = pos.y;
        mtxQuat[14] = pos.z;
        float mtxPos[16];
        bx::mtxScale(mtxPos, scale.x, scale.y, scale.z);
        float mtx[16];
        bx::mtxMul(mtx, mtxQuat, mtxPos);

        bgfx::setState(0
                       | BGFX_STATE_WRITE_RGB
                       | BGFX_STATE_WRITE_A
                       | BGFX_STATE_WRITE_Z
                       | BGFX_STATE_DEPTH_TEST_LESS
                       | BGFX_STATE_CULL_CCW
                       | BGFX_STATE_MSAA);
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, mesh->mesh->VBH);
        bgfx::setIndexBuffer(mesh->mesh->IBH);

        if (!mesh->textures.empty()) {
            if (bgfx::isValid(mesh->textures[0])) {
                bgfx::setTexture(0, s_texColor, mesh->textures[0]);
            }
            if (bgfx::isValid(mesh->textures[1])) {
                bgfx::setTexture(1, s_texNormal, mesh->textures[1]);
            }
        }

        bgfx::submit(0, mesh->material, 0);
    }

    const bgfx::Caps *caps = bgfx::getCaps();
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

#pragma omp parallel for default(none) shared(transformList,data)
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
            if (bgfx::isValid(texturesList[0])) {
                bgfx::setTexture(0, s_texColor, texturesList[0]);
            }
            if (bgfx::isValid(texturesList[1])) {
                bgfx::setTexture(1, s_texNormal, texturesList[1]);
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
        std::cout << "registering new " << cMesh->material.idx << std::endl;
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
// Create texture sampler uniforms.
    s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
}
