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
#include "../Rendering/Material.h"
#include "../Core/Box.h"


namespace Engine {

    class CMeshRenderer : public Component {
    public :
        CMeshRenderer();

        CMeshRenderer *setMesh(std::shared_ptr<BaseMesh>, CTransformMobility mobility, bool _isInstanced = false);

        CMeshRenderer *setMaterial(bgfx::ProgramHandle _material, int texAmount = 0);
        CMeshRenderer *setMaterial(std::shared_ptr<MaterialInstance> mat);

        const std::shared_ptr<MaterialInstance> &getMaterialInst() const;

        CMeshRenderer *setMaterialTexId(int idx, std::shared_ptr<Texture> texHandle);

        Box getBBox();

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
        std::shared_ptr<MaterialInstance> materialInst;

        bgfx::ProgramHandle material = BGFX_INVALID_HANDLE;
        std::vector<std::shared_ptr<Texture>> textures;
        CTransformMobility mobility;
        bool isInstanced;

        void clearMesh();
    };
}


#endif //ENGINE_CMESHRENDERER_H
