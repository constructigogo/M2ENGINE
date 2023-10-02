/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include "Engine/Core/App.h"
#include "entry/entry.h"
#include "Engine/Core/Data.h"
#include <iostream>
#include "Engine/Implicits/ImplicitTriangulate.h"
#include "Engine/Implicits/Sphere.h"
#include "Engine/Implicits/Operators/Operators.h"
#include "Engine/Components/CDirectionalLight.h"
#include "Engine/Rendering/Material.h"
#include "Engine/Core/Log.h"

class EditorSandbox : public App {
public:
    EditorSandbox(int x, int y, const char *name) : App(x, y, name) {
    }

    void init() override {
        App::init();
        currentCamera = new OCamera();
        editorScene->addObject(currentCamera);
        debugProgram = bgfx::createProgram(
                Data::loadShaderBin("v_simple.vert"),
                Data::loadShaderBin("f_simple.frag"),
                true
        );

        auto light = editorScene->createObject("Directional light");
        light->addComponent<CDirectionalLight>();


        auto instProg = bgfx::createProgram(
                Data::loadShaderBin("v_simple_inst.vert"),
                Data::loadShaderBin("f_simple_inst.frag"),
                true
        );

        auto textured = Data::loadProgram("v_textured.vert", "f_textured.frag");
        auto texMaterial = Material("textured", textured);
        auto debugMaterial = Material("debug", debugProgram);

        auto texturedInst = bgfx::createProgram(
                Data::loadShaderBin("v_textured_inst.vert"),
                Data::loadShaderBin("f_textured.frag"),
                true
        );

        auto texHandleDiffuse = Data::loadTexture("data/diffuse.jpg", Texture::TYPE::COLOR);
        auto texHandleNormal = Data::loadTexture("data/normal.png", Texture::TYPE::NORMAL);

        ENGINE_INFO("Instantiating implicit");
        auto imp = editorScene->createObject("Implicit");
        ImplicitTriangulate tr;
        Implicit *sphere = new Sphere({0.0, 0.0, 0.0}, 1.f);
        Implicit *form = new DifferenceSmooth(
                sphere,
                new Sphere({1.0, 0.4, 0.0}, 1.f),
                1.0f
        );
        float s = 2;
        auto msh = (tr.Triangulate(form,
                                   256,
                                   Box({-s, -s, -s}, {s, s, s})
        ));
        imp->addComponent<CMeshRenderer>()
                ->setMesh(msh, STATIC, false)
                ->setMaterial(debugMaterial.createInstance());



        /*
        auto inst = editorScene->createObject();
        inst->setName("Backpack");
        auto transform = inst->getComponent<CTransform>();
        transform->setPosition({0.0, 0.0, 0.0});
        //transform->setScale({0.1,0.1,0.1});
        transform->setScale(1.0);
        //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
        auto mRenderer = inst->addComponent<CMeshRenderer>()
                ->setMesh(Data::loadMesh("data/backpack.obj", false), STATIC, false)
                ->setMaterial(texMaterial.createInstance());
        auto matInst = mRenderer->getMaterialInst();
        matInst->setTexture(texHandleDiffuse);
        matInst->setTexture(texHandleNormal);
         */
        //testMesh->addComponent<CRigidBody>();




        auto res = Data::loadScene("data/export.obj");
        //for (auto obj:res) {
        //editorScene->addObject(obj);
        //}

        /*
        auto imp = editorScene->createObject();
        ImplicitTriangulate tr;
        Implicit * sphere = new Sphere({0.0,0.0,0.0}, 1.f);
        Implicit* form = new Union(
                sphere,
                new Sphere({1.0,0.4,0.0}, 1.f)
                );
        float s = 4;
        auto msh = (tr.Triangulate(form,
                       256,
                       Box({-s,-s,-s},{s,s,s})
                       ));
        auto transform = imp->addComponent<CTransform>();
        transform->setScale(0.3);
        imp->addComponent<CMeshRenderer>()
                ->setMesh(msh, STATIC, false)
                ->setMaterial(debugProgram, 0);

        */

        for (int i = 0; i < 0; ++i) {
            for (int j = 0; j < 3; ++j) {
                auto inst = editorScene->createObject();
                auto transform = inst->addComponent<CTransform>();
                transform->setPosition({(float) i * 2 - 9, -4, (float) j * 2 - 2});
                //transform->setScale({0.1,0.1,0.1});
                transform->setScale({0.1, 0.1, 0.1});
                //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
                inst->addComponent<CMeshRenderer>()
                        ->setMesh(Data::loadMesh("data/backpack.obj", false), STATIC, true)
                        ->setMaterial(texturedInst)
                        ->setMaterialTexId(0, texHandleDiffuse)
                        ->setMaterialTexId(1, texHandleNormal);
                //testMesh->addComponent<CRigidBody>();
            }
        }
    }
};


int _main_(int, char **) { return 0; }

int main(int _argc, const char *const *_argv) {
    EditorSandbox application(1424, 768, "useless");

    application.init();
    application.run();
    application.cleanup();
    return 0;
}
