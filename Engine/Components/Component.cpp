//
// Created by Constantin on 22/07/2023.
//

#include "Component.h"
#include "dear-imgui/imgui.h"
#include <algorithm>
#include <iostream>

std::vector<Engine::Component *> Engine::Component::_instances;
std::queue<Engine::Component *> Engine::Component::_startQueue;
std::set<std::type_index> Engine::Component::types;

namespace Engine {
    Component::Component() {
        Component::_instances.push_back(this);
        Component::_startQueue.push(this);
        scheduledForStart = true;
    }

    Component::~Component() {
        _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
    }

    void Component::processStart() {
        while (!Component::_startQueue.empty()) {
            Engine::Component *current = Component::_startQueue.front();
            Component::_startQueue.pop();
            if (current->scheduledForStart){
                current->start();
            }
            current->scheduledForStart = false;
            if (current->getObject()== nullptr){
                delete current;
            }
        }
    }

    void Component::processFixedUpdate(double delta) {
        for (const auto &item: _instances) {
            if (item->isActive())
                item->fixedUpdate(delta);
        }
    }

    void Component::processUpdate(double delta) {
        for (const auto &item: _instances) {
            if (item->isActive())
                item->update(delta);
        }
    }

    void Component::processLateUpdate(double delta) {
        for (const auto &item: _instances) {
            if (item->isActive())
                item->lateUpdate(delta);
        }
    }

    Object *Component::getObject() const {
        return parent;
    }

    Component::Component(Object *parent) : parent(parent) {

    }

    void Component::setParent(Engine::Object *_parent) {
        parent = _parent;
    }

    void Component::EditorUIDrawHeader() {

    }

    void Component::EditorUIDrawFoot() {

    }

    void Component::EditorUIDrawContent() {

    }

    void Component::EditorUIDraw() {

        bool target= isActive();
        if (ImGui::Checkbox("##active", &target)){
            setIsActive(target);
        }
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(viewName, ImGuiTreeNodeFlags_DefaultOpen)) {
            EditorUIDrawContent();
        }
    }

    bool Component::isActive() const {
        return _isActive;
    }

    void Component::setIsActive(bool isActive) {
        Component::_isActive = isActive;
    }
}