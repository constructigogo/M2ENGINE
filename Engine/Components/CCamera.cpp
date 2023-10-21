//
// Created by Constantin on 21/07/2023.
//

#include <iostream>
#include "CCamera.h"
#include "CTransform.h"
#include "../Core/Log.h"
#include "../Core/Ray.h"
#include "../Physic/Physic.h"
#include "../Implicits/ImplicitAll.h"

namespace Engine {
    CCamera::CCamera() {
        viewName = "Camera Controller";
    }

    void CCamera::start() {
        transform = getObject()->getComponent<CTransform>();
        if (!transform) {
            transform = getObject()->addComponent<CTransform>();
        }
        //transform->setPosition({10.0f, 10.0f, 10.0f});
        transform->setPosition({0.0, 0.0, 10.0});
        //transform->setRotation(bx::fromEuler(bx::Vec3(0.0,0.0,0.0)));
        input = new KeyInput(
                {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_R,GLFW_KEY_LEFT_SHIFT, GLFW_MOUSE_BUTTON_1});

        std::cout << "camera start" << std::endl;
    }

    void CCamera::update(double deltaTime) {
        const glm::vec3 &current = transform->getPosition();
        static float dx=0, dy=0;

        if (input->getIsKeyDown(GLFW_MOUSE_BUTTON_1)) {
            dx += -input->getMouseDeltaX() * 3.0;
            dy += input->getMouseDeltaY() * 3.0;

            dy = std::clamp(dy, glm::radians(-85.0f), glm::radians(85.0f));
            glm::quat QuatAroundX = glm::quat( glm::vec3(-dy,0.0,0.0) );
            glm::quat QuatAroundY = glm::quat( glm::vec3(0.0,dx,0.0) );
            glm::quat QuatAroundZ = glm::quat( glm::vec3(0.0,0.0,0.0));
            glm::quat finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
            transform->setRotation(finalOrientation);

        }
        if (input->getIsKeyDown(GLFW_KEY_W)) {
            transform->translate(transform->getForward() * (float) deltaTime * 5.0f);
        } else if (input->getIsKeyDown(GLFW_KEY_S)) {
            transform->translate(transform->getForward() * (float) deltaTime * -5.0f);
        }

        if (input->getIsKeyDown(GLFW_KEY_D)) {
            transform->translate(
                    glm::cross(transform->getForward(), glm::vec3(0.0, 1.0, 0.0)) * (float) deltaTime * 5.0f);
        } else if (input->getIsKeyDown(GLFW_KEY_A)) {
            transform->translate(
                    glm::cross(transform->getForward(), glm::vec3(0.0, 1.0, 0.0)) * (float) deltaTime * -5.0f);
        }

        auto localup = glm::cross(transform->getForward(),
                                  glm::cross(glm::vec3(0.0, 1.0, 0.0), transform->getForward()));

        if (input->getIsKeyDown(GLFW_KEY_E)) {
            transform->translate(localup * (float) deltaTime * 5.0f);
        } else if (input->getIsKeyDown(GLFW_KEY_Q)) {
            transform->translate(localup * (float) deltaTime * -5.0f);
        }

        if(input->getIsKeyPressed(GLFW_KEY_R)){
            ENGINE_DEBUG("Raycast from camera");
            glm::vec2 mousePos= input->getMousePosition();
            Ray r(getTransform()->getPosition(),getTransform()->getForward());
            RayHit hit{};
            if(Physic::raycast(r,hit)){
                auto rb = hit.hitRef->getComponent<CRigidBody>();
                auto rend = hit.hitRef->getComponent<CMeshRenderer>();
                ImplicitTriangulate tr;
                Implicit * form = rb->getImplCollider();
                Implicit * newForm= new DifferenceSmooth(form,new Sphere(hit.hitPosition,0.2));
                Box bbox = rend->getBBox();
                if(input->getIsKeyDown(GLFW_KEY_LEFT_SHIFT)){
                    auto msh = (tr.Triangulate(newForm,
                                               512,
                                               bbox)
                    );

                    rb->setImplCollider(newForm);
                    rend->setMesh(msh,STATIC,false);
                }
                else {
                    auto msh = (tr.Triangulate(newForm,
                                               128,
                                               bbox)
                    );

                    rb->setImplCollider(newForm);
                    rend->setMesh(msh,STATIC,false);
                }

            }
        }

        float angle = (M_PI / 180.0) * deltaTime * 10.f;

        /*bx::Vec3 rotated(current.x * cos(angle) - current.z * sin(angle),
                         current.y,
                         current.x * sin(angle)  + current.z * cos(angle));
        //rotated *=deltaTime;
        transform->setPosition(rotated);*/
    }

    const CCamera::ViewData &CCamera::getData() const {
        return data;
    }

    CTransform *CCamera::getTransform() const {
        return transform;
    }


}


