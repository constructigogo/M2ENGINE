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
#include "Engine/Rendering/MeshBuilder.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Bezier.h"
#include "Engine/Components/CRigidBody.h"

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

        auto textured = Data::loadProgram("v_textured.vert", "f_textured.frag");
        auto texMaterial = Material("textured", textured);
        auto debugMaterial = Material("debug", debugProgram);

        Material::Default=debugMaterial;
        Material::Debug=debugMaterial;

        auto res = Data::loadScene("data/exterior.obj");
        editorScene->addObjects(res);
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
