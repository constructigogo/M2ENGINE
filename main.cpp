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


        auto instProg = bgfx::createProgram(
                Data::loadShaderBin("v_simple_inst.vert"),
                Data::loadShaderBin("f_simple_inst.frag"),
                true
        );

        auto textured = Data::loadProgram("v_textured.vert", "f_textured.frag");
        auto texMaterial = Material("textured", textured);
        auto debugMaterial = Material("debug", debugProgram);

        Material::Default=debugMaterial;
        Material::Debug=debugMaterial;



        auto texturedInst = bgfx::createProgram(
                Data::loadShaderBin("v_textured_inst.vert"),
                Data::loadShaderBin("f_textured.frag"),
                true
        );

        //auto texHandleDiffuse = Data::loadTexture("data/diffuse.jpg", Texture::TYPE::COLOR);
        //auto texHandleNormal = Data::loadTexture("data/normal.png", Texture::TYPE::NORMAL);


        //ENGINE_INFO("Instantiating implicit");
        //ImplicitTriangulate tr;
        //float s = 20;
        /*
       std::vector<std::vector<glm::vec3>> pts;
       pts.push_back({glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.5, 0.0), glm::vec3(2.0, 0.0, 0.0)});
       pts.push_back({glm::vec3(0.0, 0.5, 1.0), glm::vec3(1.0, 3.0, 1.0), glm::vec3(2.0, 0.5, 1.0)});
       pts.push_back({glm::vec3(0.0, 0.0, 2.0), glm::vec3(1.0, 0.5, 2.0), glm::vec3(2.0, 0.0, 2.0)});

       Bezier curve(pts);

       for (int i = 2; i < 8; ++i) {
           auto tst_res = MeshBuilder::Polygonize(curve, std::pow(2,i));
           Data::exportToOBJ(tst_res,"surface"+std::to_string(std::pow(2,i)));
       }
       auto tst_res = MeshBuilder::Polygonize(curve, 4);
       auto curveInst = editorScene->createObject();
       auto curvTr = curveInst->getComponent<CTransform>();
       curvTr->setScale(10.0f);
       auto curveMRenderer = curveInst->addComponent<CMeshRenderer>()
               ->setMesh(tst_res, STATIC, false)
               ->setMaterial(debugMaterial.createInstance());
       */
        /*
        Bezier curve({glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0, 0.5, 0.0),glm::vec3(3.0, 1.0, 0.0)});
        Bezier axis({glm::vec3(0.0,0.0,0.0),glm::vec3(1.0,0.0,0.0)});


        auto tst_res = MeshBuilder::PolygonizeRevolution(axis,curve, 32);
        auto curveInst = editorScene->createObject();
        auto curvTr = curveInst->getComponent<CTransform>();
        curvTr->setScale(3.0f);
        auto curveMRenderer = curveInst->addComponent<CMeshRenderer>()
                ->setMesh(tst_res, STATIC, false)
                ->setMaterial(debugMaterial.createInstance());
        */
        HeightField landscape = Data::loadHeightFieldFromImage("mymap.png",30);
        editorScene->setLandscapeAsSingle(landscape,0.1,0.1,0.1);
        landscape = landscape.thermalErosion(45.0,0.001,1000);
        editorScene->setLandscapeAsSingle(landscape,0.1,0.1,0.1);

        for (int i = 0; i < 0; ++i) {
            auto inst = editorScene->createObject();
            inst->setName("Backpack");
            auto transform = inst->getComponent<CTransform>();
            transform->setPosition({-1.8+1.8, 0.3-0.7, 1.2-3.3+10.9});
            //transform->setScale({0.1,0.1,0.1});
            transform->setScale(0.5);
            //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
            auto mRenderer = inst->addComponent<CMeshRenderer>()
                    ->setMesh(Data::loadMesh("data/backpack.obj", true), STATIC, false)
                    ->setMaterial(debugMaterial.createInstance());
            auto matInst = mRenderer->getMaterialInst();
        }


        //testMesh->addComponent<CRigidBody>();


        //auto res = Data::loadScene("data/exterior.obj");
        //editorScene->addObjects(res);

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
