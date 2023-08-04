//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <vector>
#include "assimp/scene.h"
#include "bx/math.h"
#include "glm/glm.hpp"
#include "bgfx/bgfx.h"
#include "Components/CTransform.h"

namespace Engine {
    class BaseMesh {
    private:
        struct vertexData {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;
            uint32_t m_abgr;
        };

        struct SubMesh {
            SubMesh() = default;

            ~SubMesh();

            bool init(const std::vector<vertexData> &Vertices,
                      const std::vector<uint16_t> &Indices);

            std::vector<vertexData> vertexesData;
            std::vector<uint16_t> indices;


            bgfx::VertexLayout vly;
            bgfx::VertexBufferHandle VBH;
            bgfx::IndexBufferHandle IBH;
            unsigned int NumVertices;
            unsigned int MaterialIndex;
        };


        bgfx::VertexLayout vly;

    public:
        ~BaseMesh();

        std::vector<SubMesh> subMeshes;

        void loadMesh(const std::string &Filename);
        std::vector<vertexData> vertexesAllData;
        std::vector<uint16_t> indicesAllData;
        bgfx::VertexBufferHandle VBH;
        bgfx::IndexBufferHandle IBH;

    private:
        void initFromScene(const aiScene *pScene, const std::string &Filename);

        void initMesh(unsigned int Index, const aiMesh *paiMesh);


    };

    class SkeletalMesh : public BaseMesh {

    };

    class StaticMesh : public BaseMesh {

    };

}


#endif //ENGINE_MESH_H
