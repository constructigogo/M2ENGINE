//
// Created by constantin on 21/10/23.
//

#pragma once
#include "Mesh.h"
#include "../Core/Bezier.h"

namespace Engine {
    class MeshBuilder {
    public:
        MeshBuilder() = delete;

        static std::shared_ptr<BaseMesh> Polygonize(const Bezier& bezier, int n);
        static std::shared_ptr<BaseMesh> PolygonizeRevolution(const Bezier& axis,const Bezier& bezier, int n);


    protected:

    private:
    };

}

