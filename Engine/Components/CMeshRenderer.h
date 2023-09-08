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

    enum TEXTURE_TYPE{
        COLOR,
        NORMAL,
    };

    class CMeshRenderer : public Component {
    public :
        CMeshRenderer();

        CMeshRenderer* setMesh(std::shared_ptr<BaseMesh>, CTransformMobility mobility, bool _isInstanced = false);

        CMeshRenderer* setMaterial(bgfx::ProgramHandle _material, int texAmount=0);
        CMeshRenderer* setMaterialTexId(int idx, bgfx::TextureHandle texHandle);

        void start() override;

        void cleanup() override;

        ~CMeshRenderer() override;

    private :
        friend class Renderer;

        friend class CRigidBody;

        std::shared_ptr<BaseMesh> mesh;
        CTransform *transform;
        bgfx::ProgramHandle material;
        std::vector<bgfx::TextureHandle> textures;
        CTransformMobility mobility;
        bool isInstanced;

        void clearMesh();
    };
}


#endif //ENGINE_CMESHRENDERER_H
