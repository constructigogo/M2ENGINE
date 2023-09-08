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
    EditorSandbox(int x, int y, const char *name) : App(x, y, name) {
    }

    void init() override {
        App::init();
        currentCamera = new OCamera();
        debugProgram = bgfx::createProgram(
                Data::loadShaderBin("v_simple.vert"),
                Data::loadShaderBin("f_simple.frag"),
                false
        );

        auto instProg = bgfx::createProgram(
                Data::loadShaderBin("v_simple_inst.vert"),
                Data::loadShaderBin("f_simple_inst.frag"),
                false
        );

        auto textured = bgfx::createProgram(
                Data::loadShaderBin("v_textured.vert"),
                Data::loadShaderBin("f_textured.frag"),
                false
        );

        auto texDiffuse = Data::loadTexture("data/diffuse.jpg");
        auto texNormal = Data::loadTexture("data/normal.png");
        assert(bgfx::isValid(texDiffuse));
        assert(bgfx::isValid(texNormal));

        auto inst = new Object();
        auto transform = inst->addComponent<CTransform>();
        transform->setPosition({0.5, -0.5, 0.0});
        //transform->setScale({0.1,0.1,0.1});
        transform->setScale(.3);
        //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
        inst->addComponent<CMeshRenderer>()
                ->setMesh(Data::loadMesh("data/backpack.obj"), STATIC, false)
                ->setMaterial(textured,2)
                ->setMaterialTexId(0, texDiffuse)
                ->setMaterialTexId(1, texNormal);
        //testMesh->addComponent<CRigidBody>();


        for (int i = 0; i < 0; ++i) {
            for (int j = 0; j < 0; ++j) {
                auto inst = new Object();
                auto transform = inst->addComponent<CTransform>();
                transform->setPosition({(float)i*2 - 2, -0.5, (float)j*2 - 2});
                //transform->setScale({0.1,0.1,0.1});
                transform->setScale({0.1, 0.1, 0.1});
                //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
                inst->addComponent<CMeshRenderer>()
                        ->setMesh(Data::loadMesh("data/backpack.obj"), STATIC, false)
                        ->setMaterial(instProg);
                //testMesh->addComponent<CRigidBody>();
            }
        }
    }
};


int _main_(int, char **) { return 0; }

int main(int _argc, const char *const *_argv) {
    EditorSandbox application(1024, 768, "useless");

    application.init();
    application.run();
    application.cleanup();
    return 0;
}
