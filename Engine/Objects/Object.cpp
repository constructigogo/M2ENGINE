//
// Created by Constantin on 22/07/2023.
//

#include "Object.h"

namespace Engine {
    uint64_t Object::count =0;

    Object::~Object() {
        for (auto comp : m_components) {
            comp.second->cleanup() ;
        }
        for (auto comp : m_components) {
            delete comp.second ;
        }
    }

    Object::Object() {
        UID=count++;
    }

    uint64_t Object::getUid() const {
        return UID;
    }

    Object::Object(const std::string &name) : name(name) {
        Object();
    }
    Object::Object(char *name) : Object(std::string(name)){}

    const std::multimap<std::type_index, Component *> &Object::getComponents() const {
        return m_components;
    }

    const std::string &Object::getName() const {
        return name;
    }

    void Object::setName(const std::string &name) {
        Object::name = name;
    }

    bool Object::isActive() const {
        return _isActive;
    }

    void Object::setIsActive(bool isActive) {
        Object::_isActive = isActive;
        /*
        for (const auto & [key,comp] : m_components) {
            comp->setIsActive(isActive);
        }
        */
    }

}
