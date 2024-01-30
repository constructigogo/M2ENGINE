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
        static int steps = 100;
        ImGui::Text("dt : 0.001");
        ImGui::DragFloat("Angle",&angle,0.25,1.0,85.0f,"%.2f");
        ImGui::DragInt("Step count",&steps,1.0f,1,10000);
        if(ImGui::Button("Thermal Erosion")){
            static int count=0;
            ENGINE_DEBUG("Erosion Button");
            _hf = std::make_shared<HeightField>(_hf->thermalErosion(angle,0.001,steps));
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