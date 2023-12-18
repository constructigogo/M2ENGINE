//
// Created by Constantin on 04/08/2023.
//

#ifndef ENGINE_PHYSIC_H
#define ENGINE_PHYSIC_H

#include "../Components/CRigidBody.h"
#include "../Core/App.h"
#include "../Core/Ray.h"
#include "../Components/CParticleContainer.h"

namespace Engine {

    class App;
    class CRigidBody;

    class Physic {
    public:
        static bool raycast(const Ray & ray, RayHit& hit);
    private:
        friend App;
        friend CRigidBody;

        static void init();
        static void compute(double delta);


        static void registerBody(CRigidBody * body);
        static std::vector<CRigidBody*> bodies;
        static std::vector<CParticleContainer*> particles;
        static float e;
    };

} // Engine

#endif //ENGINE_PHYSIC_H
