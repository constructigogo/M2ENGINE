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

void Engine::CTransform::setPosition(const float &x, const float &y, const float &z) {
    setPosition({x, y, z});
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

void Engine::CTransform::translate(const bx::Vec3 &position) {
    Position.x += position.x;
    Position.y += position.y;
    Position.z += position.z;
}

void Engine::CTransform::translate(const float &x, const float &y, const float &z) {
    Position.x += x;
    Position.y += y;
    Position.z += z;
}

void Engine::CTransform::translate(const glm::vec3 &position) {
    Position.x += position.x;
    Position.y += position.y;
    Position.z += position.z;
}

void Engine::CTransform::setScale(const bx::Vec3 &scale) {
    Scale = scale;
}

void Engine::CTransform::setScale(const float &x, const float &y, const float &z) {
    setScale({x, y, z});
}

void Engine::CTransform::setRotation(const bx::Quaternion &rotation) {
    Rotation = rotation;
}

void Engine::CTransform::setScale(const float &xyz) {
    setScale({xyz, xyz, xyz});
}

Engine::CTransform::CTransform() {
    viewName = "Transform";
}

void Engine::CTransform::EditorUIDrawContent() {
    auto flags =
            ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_CharsDecimal |
            ImGuiInputTextFlags_CharsNoBlank
            ;

    ImGui::DragFloat3("Position", &Position.x, 0.1f,0.0f,0.0f,"%0.1f");
    auto vec = mul(bx::toEuler(Rotation), 360.f);
    if (ImGui::DragFloat3("Rotation", &vec.x, 0.1f,0.0f,0.0f,"%0.1f")) {
        setRotation(bx::fromEuler(div(vec, 360.f)));
    }
    ImGui::DragFloat3("Scale", &Scale.x, 0.1f,0.0f,0.0f,"%0.1f");

    /*
    ImGui::InputFloat3("Position", &Position.x, "%.1f", flags);
    auto vec = mul(bx::toEuler(Rotation), 360.f);
    if (ImGui::InputFloat3("Rotation", &vec.x, "%.1f", flags)) {
        setRotation(bx::fromEuler(div(vec, 360.f)));
    }
    ImGui::InputFloat3("Scale", &Scale.x, "%.1f", flags);
    */


}




