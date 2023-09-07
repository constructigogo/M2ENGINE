//
// Created by Constantin on 22/07/2023.
//

#include "CMeshRenderer.h"
#include "../Renderer.h"
#include "CTransform.h"

CMeshRenderer::CMeshRenderer() {

}

void CMeshRenderer::start() {
    transform = getParent()->getComponent<CTransform>();
    if(!transform){
        transform=getParent()->addComponent<CTransform>();
    }
}

CMeshRenderer *  CMeshRenderer::setMesh(std::shared_ptr<BaseMesh> _mesh, CTransformMobility mobility,
                                        bool isInstanced) {
    clearMesh();
    bool hasTransform = getParent()->getComponent<CTransform>() != nullptr;
    if (hasTransform){
        CMeshRenderer::mesh = _mesh;

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

CMeshRenderer *CMeshRenderer::setMaterial(bgfx::ProgramHandle _material) {
    material=_material;
    return this;
}




