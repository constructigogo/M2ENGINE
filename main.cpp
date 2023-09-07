/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include "Engine/App.h"
#include "entry/entry.h"
#include "Engine/Data.h"
#include <iostream>

class EditorSandbox : public App {
public:
    EditorSandbox(int x, int y, const char * name) : App(x, y, name) {
    }

    void init() override {
        App::init();
        currentCamera = new OCamera();
        debugProgram = bgfx::createProgram(
                Data::loadShaderBin("v_simple.vert"),
                Data::loadShaderBin("f_simple.frag"),
                false
        );
        testMesh = new Object();
        auto transform = testMesh->addComponent<CTransform>();
        transform->setPosition({0.0, -0.5, 0.0});
        //transform->setScale({0.1,0.1,0.1});
        transform->setScale({0.4,0.4,0.4});
        transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0}));
        testMesh->addComponent<CMeshRenderer>()
                ->setMesh(Data::loadMesh("data/bbox.obj"), MOVABLE)
                ->setMaterial(debugProgram);
        //testMesh->addComponent<CRigidBody>();
    }
};


int _main_(int, char**) { return 0;}

int main(int _argc, const char* const* _argv)
{
    EditorSandbox application(1024, 768, "useless");

    application.init();
    application.run();
    application.cleanup();
    return 0;
}
