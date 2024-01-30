//
// Created by constantin on 30/01/24.
//

#pragma once

#include "CMeshRenderer.h"
#include "CTransform.h"
#include "../Core/HeightField.h"

namespace Engine {

    class CLandscape : public Component {
    public:
        CLandscape();

        void start() override;

        const std::shared_ptr<HeightField> &getHf() const;

        CLandscape *setHf(const std::shared_ptr<HeightField> &hf);

    protected:
        void EditorUIDrawContent() override;

        std::shared_ptr<HeightField> _hf;
        CMeshRenderer *_rend;

    private:
    };

} // Engine
