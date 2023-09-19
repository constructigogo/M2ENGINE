//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_CMESHRENDERER_H
#define ENGINE_CMESHRENDERER_H


#include "Component.h"
#include "../Rendering/Mesh.h"
#include "CTransform.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/Texture.h"


namespace Engine {

    enum TEXTURE_TYPE {
        COLOR,
        NORMAL,
    };

    class CMeshRenderer : public Component {
    public :
        CMeshRenderer();

        CMeshRenderer *setMesh(std::shared_ptr<BaseMesh>, CTransformMobility mobility, bool _isInstanced = false);

        CMeshRenderer *setMaterial(bgfx::ProgramHandle _material, int texAmount = 0);

        CMeshRenderer *setMaterialTexId(int idx, std::shared_ptr<Texture> texHandle);


        void start() override;

        void cleanup() override;

        ~CMeshRenderer() override;

    protected:
        void EditorUIDrawContent() override;

    private :
        friend class Renderer;

        friend class CRigidBody;

        std::shared_ptr<BaseMesh> mesh;
        CTransform *transform;
        bgfx::ProgramHandle material = BGFX_INVALID_HANDLE;
        std::vector<std::shared_ptr<Texture>> textures;
        CTransformMobility mobility;
        bool isInstanced;

        void clearMesh();
    };
}


#endif //ENGINE_CMESHRENDERER_H
