//
// Created by Constantin on 04/08/2023.
//

#ifndef ENGINE_PHYSIC_H
#define ENGINE_PHYSIC_H

#include "Components/CRigidBody.h"
#include "App.h"

namespace Engine {

    class App;
    class CRigidBody;

    class Physic {
    public:

    private:
        friend App;
        friend CRigidBody;

        static void init();
        static void compute(double delta);

        static void registerBody(CRigidBody * body);
        static std::vector<CRigidBody*> bodies;
    };

} // Engine

#endif //ENGINE_PHYSIC_H
