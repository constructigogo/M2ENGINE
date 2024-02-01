//
// Created by constantin on 30/01/24.
//

#include "CLandscape.h"
#include "../Rendering/MeshBuilder.h"
#include "../Core/Data.h"
#include <utility>

namespace Engine {
    CLandscape::CLandscape() {
        viewName = "Landscape";
    }

    void CLandscape::EditorUIDrawContent() {
        static float angle = 45.0f;
        static float k =1.0;
        static float dt =0.001;
        static int steps = 100;
        ImGui::DragFloat("dt",&dt,0.001,1.0,0.001f,"%.3f");
        ImGui::DragFloat("Angle",&angle,0.25,1.0,85.0f,"%.2f");
        ImGui::DragFloat("k factor",&k,0.001,1.0,0.001f,"%.3f");
        ImGui::DragInt("Step count",&steps,1.0f,1,10000);

        static int count=0;
        if(ImGui::Button("Thermal Erosion")){
            ENGINE_DEBUG("Erosion Button");
            _hf = std::make_shared<HeightField>(_hf->thermalErosion(angle,dt,steps));
            auto msh = MeshBuilder::Polygonize(*_hf);
            std::string name="generatedLandscapeTherm"+std::to_string(count++);
            Data::trackMesh(msh,name);
            Data::invalidateCache();
            _rend->setMesh(msh,STATIC);
        }
        if(ImGui::Button("Hill Slope")){
            ENGINE_DEBUG("Hill Slope Button");

            _hf = std::make_shared<HeightField>(_hf->hillSlope(k, dt, steps));
            auto msh = MeshBuilder::Polygonize(*_hf);
            std::string name="generatedLandscapeTherm"+std::to_string(count++);
            Data::trackMesh(msh,name);
            Data::invalidateCache();
            _rend->setMesh(msh,STATIC);
        }
        if(ImGui::Button("Stream Power")){
            ENGINE_DEBUG("Stream Power Button");

            _hf = std::make_shared<HeightField>(_hf->streamPower(k/2.0f, k, dt, steps));
            auto msh = MeshBuilder::Polygonize(*_hf);
            std::string name="generatedLandscapeTherm"+std::to_string(count++);
            Data::trackMesh(msh,name);
            Data::invalidateCache();
            _rend->setMesh(msh,STATIC);
        }
        if(ImGui::Button("Reset")){
            ENGINE_DEBUG("Reset Button");
            HeightField landscape = Data::loadHeightFieldFromImage("mymap.png",30);

            _hf = std::make_shared<HeightField>(landscape);
            auto msh = MeshBuilder::Polygonize(*_hf);
            std::string name="generatedLandscapeTherm"+std::to_string(count++);
            Data::trackMesh(msh,name);
            Data::invalidateCache();
            _rend->setMesh(msh,STATIC);
        }

    }

    void CLandscape::start() {
        Component::start();
        _rend = getObject()->getComponent<CMeshRenderer>();
    }

    const std::shared_ptr<HeightField> &CLandscape::getHf() const {
        return _hf;
    }

    CLandscape *CLandscape::setHf(const std::shared_ptr<HeightField> &hf) {
        _hf = hf;
        return this;
    }


} // Engine