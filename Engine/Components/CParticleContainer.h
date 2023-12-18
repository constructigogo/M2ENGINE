//
// Created by constantin on 15/12/23.
//

#pragma once

#include "Component.h"
#include "CTransform.h"
#include "../Physic/PBDContainer.h"

namespace Engine {

    class CParticleContainer : public Component {
    public:
        CParticleContainer();

        void start() override;

        void fixedUpdate(double delta) override;

        void update(double delta) override;

        std::unique_ptr<PBDContainer> &getContainer() { return particleContainer; }

    protected:
        void EditorUIDrawContent() override;

    protected:
        CTransform *transform;
        int subStepCount = 3;
        std::unique_ptr<PBDContainer> particleContainer;
    private:
    };

} // Engine
