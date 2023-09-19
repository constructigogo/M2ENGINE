//
// Created by constantin on 13/09/23.
//

#pragma once

#include "../Rendering/Mesh.h"
#include "Implicit.h"
#include "../Core/Box.h"

namespace Engine {

    class ImplicitTriangulate {

        friend BaseMesh;

    public:
        ImplicitTriangulate() = default;

        std::shared_ptr<BaseMesh> Triangulate(Implicit *form, int n, Box bbox);
        std::shared_ptr<BaseMesh> generated;
    protected:

        Implicit * form;

        float Value(const glm::vec3& p) const;
        glm::vec3 Gradient(const glm::vec3& p) const;

        // Normal
        glm::vec3 Normal(const glm::vec3& p) const;

        glm::vec3 Dichotomy(glm::vec3 a, glm::vec3 b, float va, float vb, float length, const float& epsilon);

        static int TriangleTable[256][16]; //!< Two dimensionnal array storing the straddling edges for every marching cubes configuration.
        static int EdgeTable[256];    //!< Array storing straddling edges for every marching cubes configuration.
        static float Epsilon;
    private:
    };

} // Engine
