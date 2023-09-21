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

    void Object::setName(const std::string &_name) {
        Object::name = _name;
    }

    bool Object::isActive() const {
        return _isActive;
    }

    void Object::setIsActive(bool isActive) {
        Object::_isActive = isActive;
        for (auto & child: childrens) {
            child->setIsActive(isActive);
        }
    }

    void Object::setParent(Object *_parent) {
        Object::parent = _parent;
        _parent->childrens.push_back(this);
    }

    const std::vector<Object *> &Object::getChildrens() const {
        return childrens;
    }

}
