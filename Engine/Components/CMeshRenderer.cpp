//
// Created by Constantin on 22/07/2023.
//

#include "CMeshRenderer.h"
#include "../Renderer.h"
#include "CTransform.h"

CMeshRenderer::CMeshRenderer() {

}

void CMeshRenderer::start() {
    Component::start();
    transform = getParent()->getComponent<CTransform>();
    if(!transform){
        transform=getParent()->addComponent<CTransform>();
    }

    switch (mobility) {
        case STATIC:
            if (isInstanced){
                Renderer::registerAsStatic(this);
            }
            else{
                Renderer::registerAsDynamic(this);
            }
            break;
        case MOVABLE:
            Renderer::registerAsDynamic(this);
            break;
    }

}

CMeshRenderer *  CMeshRenderer::setMesh(std::shared_ptr<BaseMesh> _mesh, CTransformMobility _mobility,
                                        bool _isInstanced) {
    clearMesh();
    bool hasTransform = getParent()->getComponent<CTransform>() != nullptr;
    if (hasTransform){
        CMeshRenderer::mesh = _mesh;
        this->mobility = _mobility;
        this->isInstanced=_isInstanced;
        if(!scheduledForStart){
            Component::_startQueue.push(this);
            scheduledForStart = true;
        }
    }

    return this;
}

void CMeshRenderer::clearMesh() {

}

void CMeshRenderer::cleanup() {
    Renderer::unregisterDynamic(this);
    Renderer::unregisterStatic(this);
}

CMeshRenderer::~CMeshRenderer() {

}

CMeshRenderer *CMeshRenderer::setMaterial(bgfx::ProgramHandle _material, int texAmount) {
    material=_material;
    textures.clear();
    textures.reserve(texAmount);
    return this;
}

CMeshRenderer *CMeshRenderer::setMaterialTexId(int idx, bgfx::TextureHandle texHandle) {
    if (idx >= textures.capacity()){
        return this;
    }
    auto it = textures.begin()+idx;
    textures.emplace(it,texHandle);
    return this;
}




