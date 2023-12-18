//
// Created by constantin on 15/12/23.
//

#include "CParticleContainer.h"
#include "../Rendering/Renderer.h"

namespace Engine {

    CParticleContainer::CParticleContainer() {
        viewName = "Particle Container";
    }


    void CParticleContainer::start() {
        transform = getObject()->getComponent<CTransform>();
        if (!transform) {
            transform = getObject()->addComponent<CTransform>();
        }

        particleContainer = std::make_unique<PBDContainer>(
                transform->getPosition(),
                1,10,10,10,
                3
        );

        Renderer::registerEmitter(this);
    }

    void CParticleContainer::fixedUpdate(double delta) {
        particleContainer->setOrigin(transform->getPosition());
        particleContainer->update(delta,subStepCount);
    }

    void CParticleContainer::update(double delta) {
    }

    void CParticleContainer::EditorUIDrawContent() {
        int flag =ImGuiInputTextFlags_EnterReturnsTrue;


        std::string txt= "Particle Count : " + std::to_string(particleContainer->data().size());
        ImGui::Text(txt.c_str());
        if(ImGui::InputInt("Sub step count", &subStepCount)){
            if (subStepCount<1) subStepCount=1;
        }

        int constraintSubStep = particleContainer->getNumConstraintSubStep();
        if(ImGui::InputInt("Constraint SS count", &constraintSubStep)){
            if (constraintSubStep<1) constraintSubStep=1;
            particleContainer->setNumConstraintSubStep(constraintSubStep);
        }

        float frc = particleContainer->getFriction();
        if(ImGui::InputFloat("Friction", &frc,0.1,1.0,"%.3f",flag)){
            if (frc<0.0) frc=0.0;
            particleContainer->setFriction(frc);
        }

        float rd = particleContainer->getParticleSize();
        if(ImGui::InputFloat("Particle Size", &rd,0.1,1.0,"%.2f",flag)){
            if (rd<0.1) rd=0.1;
            particleContainer->setParticleSize(rd);
        }

    }


} // Engine