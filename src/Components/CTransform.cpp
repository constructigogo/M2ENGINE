//
// Created by Constantin on 20/07/2023.
//

#include "CTransform.h"

bx::Vec3 Engine::CTransform::getPositionBX() const {
    return {Position.x, Position.y, Position.z};
}

void Engine::CTransform::setPosition(const bx::Vec3 &position) {
    Position = position;
}

void Engine::CTransform::setPosition(const float &x, const float &y, const float &z){
    setPosition({x,y,z});
}

const bx::Vec3 &Engine::CTransform::getPosition() const {
    return Position;
}

const bx::Quaternion &Engine::CTransform::getRotation() const {
    return Rotation;
}

const bx::Vec3 &Engine::CTransform::getScale() const {
    return Scale;
}

