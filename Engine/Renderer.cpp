//
// Created by Constantin on 20/07/2023.
//

#include <iostream>
#include "Renderer.h"

std::map<BaseMesh* , std::pair<bgfx::ProgramHandle*,std::vector<CTransform*>>> Renderer::batching;
std::vector<CMeshRenderer*> Renderer::renderList;



void Renderer::render() {
    for (const auto mesh:renderList) {

        const bx::Vec3 & pos = mesh->transform->getPosition();
        const bx::Quaternion & rot = mesh->transform->getRotation();

        float mtx[16];
        //bx::mtxTranslate(mtx, pos.x, pos.y, pos.z);
        bx::mtxFromQuaternion(mtx,rot,pos);

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
        bgfx::submit(0, mesh->material, 0);
    }

    const bgfx::Caps* caps = bgfx::getCaps();
    const bool instancingSupported = 0 != (BGFX_CAPS_INSTANCING & caps->supported);
    if (!instancingSupported){
        return;
    }
    const uint16_t instanceStride = 64;
    for (const auto& batch : batching) {
        const auto mesh = batch.first;
        const auto & batchData = batch.second;
        uint32_t drawnInst = bgfx::getAvailInstanceDataBuffer(batchData.second.size(), instanceStride);

        bgfx::InstanceDataBuffer idb;
        bgfx::allocInstanceDataBuffer(&idb, drawnInst, instanceStride);
        uint8_t* data = idb.data;
        for (const auto transform: batchData.second) {
            const auto & pos = transform->getPosition();
            const auto & rot = transform->getRotation();


            auto* mtx = (float*)data;
            //bx::mtxTranslate(mtx, pos.x, pos.y, pos.z);
            bx::mtxFromQuaternion(mtx,rot,pos);

            data += instanceStride;
        }

        bgfx::setVertexBuffer(0, mesh->VBH);
        bgfx::setIndexBuffer(mesh->IBH);
        bgfx::setInstanceDataBuffer(&idb);

        bgfx::setState(0
                       | BGFX_STATE_WRITE_RGB
                       | BGFX_STATE_WRITE_A
                       | BGFX_STATE_WRITE_Z
                       | BGFX_STATE_DEPTH_TEST_LESS
                       | BGFX_STATE_CULL_CCW
                       | BGFX_STATE_MSAA);
        bgfx::submit(0, *batchData.first, 0);

    }

    bgfx::frame();
}



void Renderer::registerAsDynamic(CMeshRenderer* cMesh) {
    std::cout << "registering dynamic "<< cMesh->material.idx<<std::endl;
    Renderer::renderList.push_back(cMesh);
}

void Renderer::registerAsStatic(CMeshRenderer * cMesh) {

    //bgfx::setInstanceCount()
    const auto key = cMesh->mesh.get();
    auto it = batching.find(cMesh->mesh.get());
    if (it != batching.end()) {
        std::cout << "registering add"<<std::endl;
        batching[key].second.push_back(cMesh->getParent()->getComponent<CTransform>());
    }
    else {
        std::cout << "registering new "<< cMesh->material.idx<<std::endl;
        batching[key].first = &cMesh->material;
        batching[key].second.push_back(cMesh->getParent()->getComponent<CTransform>());
    }

}

void Renderer::unregisterDynamic(CMeshRenderer * cMesh) {
    renderList.erase(std::remove(renderList.begin(), renderList.end(), cMesh), renderList.end());
}

void Renderer::unregisterStatic(CMeshRenderer * cMesh) {
    const auto key = cMesh->mesh.get();
    auto it = batching.find(key);
    if (it != batching.end()) {
        const auto tf = cMesh->getParent()->getComponent<CTransform>();
        auto & list = batching[key].second;
        list.erase(std::remove(list.begin(), list.end(), tf), list.end());
    }
}
