//
// Created by Constantin on 21/07/2023.
//

#include <iostream>
#include "CCamera.h"
#include "CTransform.h"

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
                {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E, GLFW_MOUSE_BUTTON_1});

        std::cout << "camera start" << std::endl;
    }

    void CCamera::update(double deltaTime) {
        const glm::vec3 &current = transform->getPosition();
        float dx, dy;

        if (input->getIsKeyDown(GLFW_MOUSE_BUTTON_1)) {
            dx = input->getMouseDeltaX() * 3.0;
            dy = input->getMouseDeltaY() * 3.0;
            auto rotD = glm::quat(glm::vec3(dy * cos(dx), dx, dy * sin(dx)));
            auto current = (transform->getRotation());
            auto res = current * rotD;
            transform->setRotation(res);


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


