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

        auto texturedInst = bgfx::createProgram(
                Data::loadShaderBin("v_textured_inst.vert"),
                Data::loadShaderBin("f_textured.frag"),
                true
        );

        auto texHandleDiffuse = Data::loadTexture("data/diffuse.jpg", Texture::TYPE::COLOR);
        auto texHandleNormal = Data::loadTexture("data/normal.png", Texture::TYPE::NORMAL);


        ENGINE_INFO("Instantiating implicit");
        ImplicitTriangulate tr;
        float s = 20;
        /**
        auto imp = editorScene->createObject("Implicit");
        Implicit *sphere = new Sphere({0.0, 0.0, 0.0}, 1.f);
        Implicit *form = new DifferenceSmooth(
                sphere,
                new Sphere({1.0, 0.4, 0.0}, 1.f),
                1.0f
        );

        auto msh = (tr.Triangulate(form,
                                   256,
                                   Box({-s, -s, -s}, {s, s, s})
        ));
        imp->addComponent<CMeshRenderer>()
                ->setMesh(msh, STATIC, false)
                ->setMaterial(debugMaterial.createInstance());
        auto rb = imp->addComponent<CRigidBody>();
        rb->setImplCollider(form);
        rb->affectedByGravity = false;
        **/
        /*
        double timer = glfwGetTime();
        auto imp2 = editorScene->createObject("ImplicitBoxFrame");
        //imp2->getComponent<CTransform>()->setRotation(glm::quat(glm::vec3(0.0,0.0,M_PI/4)));

        Implicit *turretBase =
                new Union(
                        new Union(
                                new BoxExact(glm::vec3(0.0, 0.0, 0.0), glm::vec3(), glm::vec3(1.0, 1.0, 1.4)),
                                new BoxExact(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 0.0, M_PI / 4),
                                             glm::vec3(sqrt(2.0) / 2, sqrt(2.0) / 2, 1.4))
                        ),
                        new BoxExact(glm::vec3(1.5, -0.5, 0.0), glm::vec3(), glm::vec3(0.5, 0.5, 1.4))
                );

        Implicit *barrel = new BoxExact(glm::vec3(0.0, 0.0, 0.0), glm::vec3(), glm::vec3(1.6, 0.22, 0.22));

        float vertSpread = 1.2;
        float top = vertSpread;
        float bottom = -vertSpread;

        float horSpread = 4;
        float left = horSpread;
        float right = -horSpread;
        Implicit *shipCore = new BoxExact(glm::vec3(0.0, 0.0, 0.0), glm::vec3(),
                                          glm::vec3(15.2, 3, 8.5));
        {


            shipCore = new Difference(
                    new Difference(
                            shipCore,
                            new Plane(glm::vec3(-15.2, 0.0, -0.5),
                                      glm::vec3(0.0, 0.261799, 0.0),
                                      glm::vec3(1.0, 0.0, 0.0))
                    ),
                    new Plane(glm::vec3(-15.2, 0.0, 0.5),
                              glm::vec3(0.0, -0.261799, 0.0),
                              glm::vec3(1.0, 0.0, 0.0))
            );

            Implicit *frontCut =
                    new Union(
                            new Plane(glm::vec3(15.2, 0.0, -0.7),
                                      glm::vec3(0.0, 0.261799, 0.0),
                                      glm::vec3(0.0, 0.0, 1.0)),
                            new Plane(glm::vec3(15.2, 0.0, 0.7),
                                      glm::vec3(0.0, -0.261799, 0.0),
                                      glm::vec3(0.0, 0.0, -1.0))
                    );
            frontCut = new Difference(
                    frontCut,
                    new Plane(glm::vec3(7.1 - 15.2 / 2, 0.0, 0.0),
                              glm::vec3(0.0, 0.0, 0.0),
                              glm::vec3(1.0, 0.0, 0.0))
            );
            shipCore = new Difference(
                    shipCore,
                    frontCut
            );


            Implicit *backCut =
                    new Union(
                            new Intersection(
                                    new Intersection(
                                            new Plane(glm::vec3(-0.5, 0.0, -4.95),
                                                      glm::vec3(0.0, -0.593412, 0.0),
                                                      glm::vec3(0.0, 0.0, 1.0)),
                                            new Plane(glm::vec3(1.8, 0.0, -1.8),
                                                      glm::vec3(0.0, 0.261799, 0.0),
                                                      glm::vec3(0.0, 0.0, 1.0))
                                    ),
                                    new Plane(glm::vec3(-15.2 / 2.0 + 1.4, 0.0, -4.2),
                                              glm::vec3(0.0, 0.7243116, 0.0),
                                              glm::vec3(0.0, 0.0, 1.0))
                            ),
                            new Intersection(
                                    new Intersection(
                                            new Plane(glm::vec3(-0.5, 0.0, 4.95),
                                                      glm::vec3(0.0, 0.593412, 0.0),
                                                      glm::vec3(0.0, 0.0, -1.0)),
                                            new Plane(glm::vec3(1.8, 0.0, 1.8),
                                                      glm::vec3(0.0, -0.261799, 0.0),
                                                      glm::vec3(0.0, 0.0, -1.0))
                                    ),
                                    new Plane(glm::vec3(-15.2 / 2.0 + 1.4, 0.0, 4.2),
                                              glm::vec3(0.0, -0.7243116, 0.0),
                                              glm::vec3(0.0, 0.0, -1.0))
                            )
                    );


            backCut = new Difference(
                    backCut,
                    new Plane(glm::vec3(7.1 - 15.2 / 2.0, 0.0, 0.0),
                              glm::vec3(0.0, 0.0, 0.0),
                              glm::vec3(-1.0, 0.0, 0.0))
            );
            backCut = new Union(
                    backCut,
                    new Plane(glm::vec3(-15.2, -3.0, 0.0),
                              glm::vec3(0.0, 0.0, 0.750492),
                              glm::vec3(1.0, 0.0, 0.0))
            );

            shipCore = new Difference(
                    shipCore,
                    backCut
            );
            shipCore = new Difference(
                    shipCore,
                    new Plane(glm::vec3(0.0, bottom, 0.0),
                              glm::vec3(0.191986, 0.0, -0.0610865),
                              glm::vec3(0.0, 1.0, 0.0))
            );
            shipCore = new Difference(
                    shipCore,
                    new Plane(glm::vec3(0.0, bottom, 0.0),
                              glm::vec3(-0.191986, 0.0, -0.0610865),
                              glm::vec3(0.0, 1.0, 0.0))
            );
            shipCore = new Difference(
                    shipCore,
                    new Plane(glm::vec3(0.0, top, 0.0),
                              glm::vec3(0.191986, 0.0, 0.0610865),
                              glm::vec3(0.0, -1.0, 0.0))
            );
            shipCore = new Difference(
                    shipCore,
                    new Plane(glm::vec3(0.0, top, 0.0),
                              glm::vec3(-0.191986, 0.0, 0.0610865),
                              glm::vec3(0.0, -1.0, 0.0))
            );
        }
        {
            shipCore = new Difference(
                    shipCore,
                    new BoxExact(glm::vec3(-1.6, -3.0, 0.0), glm::vec3(), glm::vec3(1.6, 2.5, 1.0))
            );
            shipCore = new Difference(
                    shipCore,
                    new BoxExact(glm::vec3(-15.2, -3.0, 0.0), glm::vec3(), glm::vec3(4.8 * 2, 1.8, 1.0))
            );
        }

        {
            Implicit *backBlock = new BoxExact(glm::vec3(-13.1, 0.0, 0.0), glm::vec3(), glm::vec3(4.2, 1.2, 0.2));
            backBlock = new Union(
                    backBlock,
                    new BoxExact(glm::vec3(-13.1, 0.8, 0.0), glm::vec3(), glm::vec3(4.2, 0.8, 0.8))
            );

            backBlock = new Difference(
                    backBlock,
                    new Plane(glm::vec3(-17.3, 0.0, 0.0),
                              glm::vec3(0.0, 0.0, 0.698132),
                              glm::vec3(1.0, 0.0, 0.0))
            );
            backBlock = new Difference(
                    backBlock,
                    new Plane(glm::vec3(-17.3, 0.0, 0.0),
                              glm::vec3(0.0, 0.0, -0.785398),
                              glm::vec3(1.0, 0.0, 0.0))
            );

            shipCore = new Union(shipCore, backBlock);
        }

        {
            Implicit *topBlock = new BoxExact(glm::vec3(2.1, 2.2, 0.0), glm::vec3(), glm::vec3(13.1, 2.0, 2.0));

            topBlock = new Difference(
                    topBlock,
                    new Plane(glm::vec3(0.0, top + 0.05, 0.0),
                              glm::vec3(0.0, 0.0, 0.0610865),
                              glm::vec3(0.0, -1.0, 0.0))
            );


            Implicit *habitat = new BoxExact(glm::vec3(-7.0, 3.0, 0.0), glm::vec3(), glm::vec3(6, 2.0, 0.5));

            {
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 2.6, 0.5),
                                          glm::vec3(0.122173, -0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, -1.0))
                        );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 2.6, -0.5),
                                          glm::vec3(-0.122173, 0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, 1.0))
                        );
            }


            habitat = new Difference(
                    habitat,
                    new Plane(glm::vec3(-6.8, 3.8, 0.0),
                              glm::vec3(0.0, 0.0, 0.436332),
                              glm::vec3(0.0, -1.0, 0.0))
            );

            {
                habitat = new Union(
                        habitat,
                        new Difference(
                                new BoxExact(glm::vec3(-7.0, 3.0, 0.0), glm::vec3(), glm::vec3(6, 2.0, 0.75)),
                                new Plane(glm::vec3(-6.8, 3.8, 0.0),
                                          glm::vec3(0.0, 0.0, 0.663225),
                                          glm::vec3(0.0, -1.0, 0.0))
                        )
                );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 2.6, 0.75),
                                          glm::vec3(0.122173, -0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, -1.0))
                        );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 2.6, -0.75),
                                          glm::vec3(-0.122173, 0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, 1.0))
                        );
            }

            {
                habitat = new Union(
                        habitat,
                        new Difference(
                                new BoxExact(glm::vec3(-7.0, 3.0, 0.0), glm::vec3(), glm::vec3(6, 2.0, 1.25)),
                                new Plane(glm::vec3(-6.8, 3.8, 0.0),
                                          glm::vec3(0.0, 0.0, 0.785398),
                                          glm::vec3(0.0, -1.0, 0.0))
                        )
                );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 4, 0.9),
                                          glm::vec3(0.122173, -0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, -1.0))
                        );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 4, -0.9),
                                          glm::vec3(-0.122173, 0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, 1.0))
                        );
            }

            habitat = new Union(
                    habitat,
                    new Difference(
                            new BoxExact(glm::vec3(-7.0, 3.0, 0.0), glm::vec3(), glm::vec3(6, 2.0, 2.0)),
                            new Plane(glm::vec3(-7.3, 3.8, 0.0),
                                      glm::vec3(0.0, 0.0, 0.785398),
                                      glm::vec3(0.0, -1.0, 0.0))
                    )
            );

            {
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 3.8, 1.1),
                                          glm::vec3(0.244346, -0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, -1.0))
                        );
                habitat =
                        new Difference(
                                habitat,
                                new Plane(glm::vec3(-7.3, 3.8, -1.1),
                                          glm::vec3(-0.244346, 0.0610865, 0.0),
                                          glm::vec3(0.0, 0.0, 1.0))
                        );
            }

            {
                habitat = new Union(
                        habitat,
                        topBlock
                );

                habitat = new Difference(
                        habitat,
                        new Plane(glm::vec3(15.2, 0.0, 0.3),
                                  glm::vec3(0.0, -0.0610865, 0.0),
                                  glm::vec3(0.0, 0.0, -1.0))
                );

                habitat = new Difference(
                        habitat,
                        new Plane(glm::vec3(15.2, 0.0, -0.3),
                                  glm::vec3(0.0, 0.0610865, 0.0),
                                  glm::vec3(0.0, 0.0, 1.0))
                );
            }

            habitat =
                    new Difference(
                            habitat,
                            new Plane(glm::vec3(0.0, 3.8, 0.0),
                                      glm::vec3(0.0, 0.0, 0.0),
                                      glm::vec3(0.0, -1.0, 0.0))
                    );


            Implicit *towers = new Union(
                    new BoxExact(glm::vec3(-8.25, 4.0, 0.65), glm::vec3(), glm::vec3(1.5, 2.0, 0.3)),
                    new BoxExact(glm::vec3(-8.25, 4.0, -0.65), glm::vec3(), glm::vec3(1.5, 2.0, 0.3))
            );

            towers = new Difference(
                    towers,
                    new Plane(glm::vec3(-6.75, 3.8, 0.0),
                              glm::vec3(0.0, 0.0, -0.314159),
                              glm::vec3(-1.0, 0.0, 0.0))
            );
            towers = new Difference(
                    towers,
                    new Plane(glm::vec3(-9.75, 3.8, 0.0),
                              glm::vec3(0.0, 0.0, 0.383972),
                              glm::vec3(1.0, 0.0, 0.0))
            );

            Implicit *cockpits = new Union(
                    new BoxExact(glm::vec3(-7.75, 6.0, 0.65), glm::vec3(), glm::vec3(2.5, 0.3, 0.4)),
                    new BoxExact(glm::vec3(-7.75, 6.0, -0.65), glm::vec3(), glm::vec3(2.5, 0.3, 0.4))
            );

            cockpits =
                    new Union(
                            cockpits,
                            new Union(
                                    new BoxExact(glm::vec3(-9.75, 6.25, 0.65), glm::vec3(), glm::vec3(1, 0.15, 0.45)),
                                    new BoxExact(glm::vec3(-9.75, 6.25, -0.65), glm::vec3(), glm::vec3(1, 0.15, 0.45))
                            )
                    );


            cockpits = new Difference(
                    cockpits,
                    new Plane(glm::vec3(-5.25, 6.0, 0.0),
                              glm::vec3(0.0, 0.0, 0.314159),
                              glm::vec3(0.0, -1.0, 0.0))
            );

            towers = new Union(
                    cockpits,
                    towers
            );


            habitat = new Difference(
                    habitat,
                    new Plane(glm::vec3(-9.75, 3.8, 0.0),
                              glm::vec3(0.0, 0.0, 0.698132),
                              glm::vec3(1.0, 0.0, 0.0))
            );

            habitat = new Union(
                    towers,
                    habitat
            );

            shipCore = new Union(habitat, shipCore);
        }

        {
            float d = 1.6;
            Implicit *reactorsRight;

            {
                reactorsRight =
                        new Union(
                                new CappedConeExact(glm::vec3(-13.0, -0.6, d),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 2.0, 0.6, 0.6),
                                new Union(
                                        new Union(
                                                new CappedConeExact(glm::vec3(-16, -0.6, d),
                                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                                    0.6, 0.8),
                                                new CappedConeExact(glm::vec3(-15.3, -0.6, d),
                                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                                    0.8, 0.6)
                                        ),
                                        new CappedConeExact(glm::vec3(-14.8, -0.6, d),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.2, 0.6, 0.8)
                                ));

                reactorsRight = new Union(
                        reactorsRight,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-13.8, -0.2, d + 1.6),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.6, 0.8),
                                        new CappedConeExact(glm::vec3(-13.1, -0.2, d + 1.6),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.8, 0.6)
                                ),
                                new CappedConeExact(glm::vec3(-12.6, -0.2, d + 1.6),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.0, 0.6, 0.6)
                        )
                );

                reactorsRight = new Union(
                        reactorsRight,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-14.2, 0.5, d - 0.35),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.3, 0.4),
                                        new CappedConeExact(glm::vec3(-13.5, 0.5, d - 0.35),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.4, 0.3)
                                ),
                                new CappedConeExact(glm::vec3(-13, 0.5, d - 0.35),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.3, 0.3)
                        )
                );

                reactorsRight = new Union(
                        reactorsRight,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-12.4, 0.5, d + 0.95),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.15, 0.2),
                                        new CappedConeExact(glm::vec3(-11.7, 0.5, d + 0.95),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.2, 0.15)
                                ),
                                new CappedConeExact(glm::vec3(-11.2, 0.5, d + 0.95),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.15, 0.15)
                        )
                );

                reactorsRight = new Union(
                        reactorsRight,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-14.4, -0.6, d - 1.1),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.15, 0.2),
                                        new CappedConeExact(glm::vec3(-13.7, -0.6, d - 1.1),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.2, 0.15)
                                ),
                                new CappedConeExact(glm::vec3(-13.2, -0.6, d - 1.1),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.15, 0.15)
                        )
                );
            }

            Implicit *reactorsLeft;
            {
                reactorsLeft =
                        new Union(
                                new CappedConeExact(glm::vec3(-13.0, -0.6, -d),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 2.0, 0.6, 0.6),
                                new Union(
                                        new Union(
                                                new CappedConeExact(glm::vec3(-16, -0.6, -d),
                                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                                    0.6, 0.8),
                                                new CappedConeExact(glm::vec3(-15.3, -0.6, -d),
                                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                                    0.8, 0.6)
                                        ),
                                        new CappedConeExact(glm::vec3(-14.8, -0.6, -d),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.2, 0.6, 0.8)
                                ));

                reactorsLeft = new Union(
                        reactorsLeft,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-13.8, -0.2, -d - 1.6),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.6, 0.8),
                                        new CappedConeExact(glm::vec3(-13.1, -0.2, -d - 1.6),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.8, 0.6)
                                ),
                                new CappedConeExact(glm::vec3(-12.6, -0.2, -d - 1.6),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.0, 0.6, 0.6)
                        )
                );

                reactorsLeft = new Union(
                        reactorsLeft,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-14.2, 0.5, -d + 0.35),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.3, 0.4),
                                        new CappedConeExact(glm::vec3(-13.5, 0.5, -d + 0.35),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.4, 0.3)
                                ),
                                new CappedConeExact(glm::vec3(-13, 0.5, -d + 0.35),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.3, 0.3)
                        )
                );

                reactorsLeft = new Union(
                        reactorsLeft,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-12.4, 0.5, -d - 0.95),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.15, 0.2),
                                        new CappedConeExact(glm::vec3(-11.7, 0.5, -d - 0.95),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.2, 0.15)
                                ),
                                new CappedConeExact(glm::vec3(-11.2, 0.5, -d - 0.95),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.15, 0.15)
                        )
                );

                reactorsLeft = new Union(
                        reactorsLeft,
                        new Union(
                                new Union(
                                        new CappedConeExact(glm::vec3(-14.4, -0.6, -d + 1.1),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.4,
                                                            0.15, 0.2),
                                        new CappedConeExact(glm::vec3(-13.7, -0.6, -d + 1.1),
                                                            glm::vec3(0.0, 0.0, M_PI / 2.0), 0.3,
                                                            0.2, 0.15)
                                ),
                                new CappedConeExact(glm::vec3(-13.2, -0.6, -d + 1.1),
                                                    glm::vec3(0.0, 0.0, M_PI / 2.0), 1.4, 0.15, 0.15)
                        )
                );
            }

            Implicit *reactors = new Union(
                    reactorsLeft,
                    reactorsRight
            );

            shipCore = new Union(reactors, shipCore);
        }


        auto msh = (tr.Triangulate(shipCore,
                                   64,
                                   Box({-s, -s / 2.0, -10}, {s, s / 2.0, 10})
        ));
        imp2->addComponent<CMeshRenderer>()
                ->setMesh(msh, STATIC, false)
                ->setMaterial(debugMaterial.createInstance());
        auto rb = imp2->addComponent<CRigidBody>();
        rb->setImplCollider(shipCore);
        for (int i = 2; i <= 10; ++i) {
            double ltimer = glfwGetTime();
            int pow = std::pow(2, i);
            auto tst_res = (tr.Triangulate(shipCore,
                                           pow,
                                           Box({-s, -s / 2.0, -10}, {s, s / 2.0, 10})
            ));
            ltimer = glfwGetTime() - ltimer;
            ENGINE_INFO("Generation time for res " + std::to_string(pow) + " : " + std::to_string(ltimer));

        }

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



        auto inst = editorScene->createObject();
        inst->setName("Backpack");
        auto transform = inst->getComponent<CTransform>();
        transform->setPosition({0.0, 0.0, 0.0});
        //transform->setScale({0.1,0.1,0.1});
        transform->setScale(4.0);
        //transform->setRotation(bx::fromEuler({0.0, 0.0, 45.0f*(i+j)}));
        auto mRenderer = inst->addComponent<CMeshRenderer>()
                ->setMesh(Data::loadMesh("data/triangle.obj", true), STATIC, false)
                ->setMaterial(debugMaterial.createInstance());
        auto matInst = mRenderer->getMaterialInst();

        //testMesh->addComponent<CRigidBody>();




        auto res = Data::loadScene("data/exterior.obj");
        for (auto obj: res) {
            editorScene->addObject(obj);
        }


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
