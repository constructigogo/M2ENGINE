//
// Created by Constantin on 21/07/2023.
//

#include <iostream>
#include "CCamera.h"
#include "CTransform.h"

Engine::CCamera::CCamera() {
}

void Engine::CCamera::start() {
    transform = getParent()->getComponent<CTransform>();
    if(!transform){
        transform=getParent()->addComponent<CTransform>();
    }
    //transform->setPosition({10.0f, 10.0f, 10.0f});
    transform->setPosition({1.0f, .2f, 1.0f});

    std::cout<<"camera start"<<std::endl;
}

void Engine::CCamera::update(double deltaTime) {
    const bx::Vec3 &current =transform->getPosition();

    float angle = (M_PI/180.0)*deltaTime*10.f;


    bx::Vec3 rotated(current.x * cos(angle) - current.z * sin(angle),
                      current.y,
                      current.x * sin(angle)  + current.z * cos(angle));
    //rotated *=deltaTime;
    transform->setPosition(rotated);
}

const Engine::CCamera::ViewData &Engine::CCamera::getData() const {
    return data;
}

Engine::CTransform *Engine::CCamera::getTransform() const {
    return transform;
}



