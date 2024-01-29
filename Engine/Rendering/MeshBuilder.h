//
// Created by constantin on 21/10/23.
//

#pragma once
#include "Mesh.h"
#include "../Core/Bezier.h"
#include "../Core/HeightField.h"

namespace Engine {
    class MeshBuilder {
    public:
        MeshBuilder() = delete;

        static std::shared_ptr<BaseMesh> Polygonize(const Bezier& bezier, int n);
        static std::shared_ptr<BaseMesh> PolygonizeRevolution(const Bezier& axis,const Bezier& bezier, int n);
        static std::shared_ptr<BaseMesh> Polygonize(const HeightField& scalarField);
        static std::vector<std::shared_ptr<BaseMesh>> Polygonize(const HeightField scalarField, float scale,int gridSize);

    protected:

    private:
    };

}

