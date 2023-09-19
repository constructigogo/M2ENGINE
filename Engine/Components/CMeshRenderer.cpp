//
// Created by Constantin on 22/07/2023.
//

#include "CMeshRenderer.h"
#include "../Rendering/Renderer.h"
#include "CTransform.h"
#include "../Core/Data.h"

CMeshRenderer::CMeshRenderer() {
    viewName = "Mesh Renderer";
}

void CMeshRenderer::start() {
    Component::start();
    transform = getObject()->getComponent<CTransform>();
    if (!transform) {
        transform = getObject()->addComponent<CTransform>();
    }

    switch (mobility) {
        case STATIC:
            if (isInstanced) {
                Renderer::registerAsStatic(this);
            } else {
                Renderer::registerAsDynamic(this);
            }
            break;
        case MOVABLE:
            Renderer::registerAsDynamic(this);
            break;
    }

}

CMeshRenderer *CMeshRenderer::setMesh(std::shared_ptr<BaseMesh> _mesh, CTransformMobility _mobility,
                                      bool _isInstanced) {
    clearMesh();
    bool hasTransform = getObject()->getComponent<CTransform>() != nullptr;
    if (hasTransform) {
        CMeshRenderer::mesh = _mesh;
        this->mobility = _mobility;
        this->isInstanced = _isInstanced;
        if (!scheduledForStart) {
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
    material = _material;
    textures.clear();
    textures.reserve(texAmount);
    return this;
}

CMeshRenderer *CMeshRenderer::setMaterialTexId(int idx, bgfx::TextureHandle texHandle) {
    if (idx >= textures.capacity()) {
        return this;
    }
    auto it = textures.begin() + idx;
    textures.emplace(it, texHandle);
    return this;
}

void CMeshRenderer::EditorUIDrawContent() {
    ImGui::SeparatorText("Mesh Settings");
    if (mesh) {
        ImGui::Text("Current mesh : %s", mesh->getName().c_str());
    } else {
        ImGui::Text("Mesh not set");
    }
    auto & array = Data::getLoadableMesh();
    static int selected = 0;
    static auto str = array.begin()->c_str();
    if (ImGui::BeginCombo("Select mesh", str, 0)) {
        int idx = 0;
        for (auto &files: Data::getLoadableMesh()) {
            if (ImGui::Selectable(files.c_str(), idx==selected)){
                selected = idx;
                str = files.c_str();
            }

            if (selected==idx)
                ImGui::SetItemDefaultFocus();

            idx++;
        }
        ImGui::EndCombo();
    }
    if(ImGui::Button("Confirm")){
        std::cout<<str<<std::endl;
        mesh = Data::loadMesh(std::string(str));
    }
    ImGui::SeparatorText("Material settings");
}




