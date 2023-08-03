//
// Created by Constantin on 22/07/2023.
//

#include "Component.h"
#include <iostream>

std::vector<Engine::Component *> Engine::Component::_instances;
std::queue<Engine::Component *> Engine::Component::_startQueue;

namespace Engine {
    Component::Component() {
        Component::_instances.push_back(this);
        Component::_startQueue.push(this);
    }

    Component::~Component() {
        _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
    }

    void Component::processStart() {
        while (!Component::_startQueue.empty()) {
            Engine::Component *current = Component::_startQueue.front();
            Component::_startQueue.pop();
            current->start();
        }
    }

    void Component::processUpdate(double delta) {
        for (const auto &item: _instances) {
            item->update(delta);
        }
    }

    void Component::processLateUpdate(double delta) {
        for (const auto &item: _instances) {
            item->lateUpdate(delta);
        }
    }

    Object *Component::getParent() const {
        return parent;
    }

    Component::Component(Object *parent) : parent(parent) {

    }

    void Component::setParent(Engine::Object *_parent) {
        parent = _parent;
    }
}