//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_CMESHRENDERER_H
#define ENGINE_CMESHRENDERER_H


#include "Component.h"
#include "../Mesh.h"
#include "CTransform.h"
#include "../Renderer.h"

namespace Engine {
    class CMeshRenderer : public Component {
    public :
        CMeshRenderer();

        CMeshRenderer *setMesh(std::shared_ptr<BaseMesh>, CTransformMobility mobility, bool isInstanced = false);

        CMeshRenderer *setMaterial(bgfx::ProgramHandle _material);

        void start() override;

        void cleanup() override;

        ~CMeshRenderer() override;

    private :
        friend class Renderer;

        friend class CRigidBody;

        std::shared_ptr<BaseMesh> mesh;
        CTransform *transform;
        bgfx::ProgramHandle material;

        void clearMesh();
    };
}


#endif //ENGINE_CMESHRENDERER_H
