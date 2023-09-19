//
// Created by constantin on 18/09/23.
//

#pragma once
#include "Component.h"

namespace Engine {
    class CDummy : public Component {
    public:
        CDummy();

    protected:
        void EditorUIDrawContent() override;

    protected:

    private:
    };


}


