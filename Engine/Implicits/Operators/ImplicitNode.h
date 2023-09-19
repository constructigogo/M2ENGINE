//
// Created by constantin on 13/09/23.
//

#pragma once

#include "../Implicit.h"

namespace Engine {

    class ImplicitNode : public Implicit {
    public:
        ImplicitNode(Implicit *l, Implicit *r);
        ImplicitNode() = delete;
        ~ImplicitNode() override =default;
    protected:
        Implicit *l;
        Implicit *r;
    private:
    };

} // Engine
