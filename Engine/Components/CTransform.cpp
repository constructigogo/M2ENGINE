//
// Created by Constantin on 20/07/2023.
//

#include "CTransform.h"

glm::vec3 Engine::CTransform::getPositionBX() const {
    return {Position.x, Position.y, Position.z};
}

void Engine::CTransform::setPosition(const glm::vec3 &position) {
    Position = position;
}

void Engine::CTransform::setPosition(const float &x, const float &y, const float &z) {
    setPosition({x, y, z});
}

const glm::vec3 &Engine::CTransform::getPosition() const {
    return Position;
}

const glm::quat &Engine::CTransform::getRotation() const {
    return Rotation;
}

const glm::vec3 &Engine::CTransform::getScale() const {
    return Scale;
}

void Engine::CTransform::translate(const glm::vec3 &position) {
    Position.x += position.x;
    Position.y += position.y;
    Position.z += position.z;
}

void Engine::CTransform::translate(const float &x, const float &y, const float &z) {
    Position.x += x;
    Position.y += y;
    Position.z += z;
}

void Engine::CTransform::setScale(const glm::vec3 &scale) {
    Scale = scale;
}

void Engine::CTransform::setScale(const float &x, const float &y, const float &z) {
    setScale({x, y, z});
}

void Engine::CTransform::setRotation(const glm::quat &rotation) {
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
    auto vec = glm::eulerAngles(Rotation) * 180.0f/glm::pi<float>();
    //auto vec = mul(bx::toEuler(Rotation),180.0/bx::kPi);
    if (ImGui::DragFloat3("Rotation", &vec.x, 0.1f,0.0f,0.0f,"%0.1f")) {
        setRotation(glm::quat(vec * glm::pi<float>()/180.0f));
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

glm::vec3 Engine::CTransform::getForward() const {
    glm::vec3 forward{0.0,0.0,-1.0};
    return glm::normalize(forward*Rotation) ;
}

glm::vec3 Engine::CTransform::getRight() const {
    glm::vec3 right{1.0,0.0,0.0};
    return glm::normalize(right*Rotation) ;

}

glm::vec3 Engine::CTransform::getUp() const {
    glm::vec3 up{0.0,1.0,0.0};
    return glm::normalize(up*Rotation) ;

}

const glm::mat4x4 Engine::CTransform::getTransformMTX() const {
    glm::mat4 model_matrix(1.0f);
    auto translate = glm::translate(model_matrix, getPosition());
    auto rotate = glm::mat4_cast(getRotation());
    auto scale = glm::scale(model_matrix, getScale());
    glm::mat4 mtx = translate * rotate * scale;
    return mtx;
}




