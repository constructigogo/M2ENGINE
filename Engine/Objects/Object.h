//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <typeindex>
#include "map"
#include "memory"
#include "iostream"
#include "../Components/Component.h"


namespace Engine {
    class Component;

    class Object {
    protected:
        std::string name = "Empty Object";
        uint64_t UID;
        static uint64_t count;
        bool _isActive = true;
        Object *parent = nullptr;
        std::multimap<std::type_index, Component *> m_components;
        std::vector<Object *> childrens;
    public:

        Object();

        Object(char *name);

        Object(const std::string &name);

        uint64_t getUid() const;

        bool isActive() const;
        void setIsActive(bool isActive);

        void setName(const std::string &_name);

        void setParent(Object *_parent);

        const std::vector<Object *> &getChildrens() const;

        const std::string &getName() const;

        const std::multimap<std::type_index, Component *> &getComponents() const;

        ~Object();

        template<typename T,
                typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
        T *getComponent() {
            auto it = m_components.find(std::type_index(typeid(T)));
            if (it == m_components.end()) return nullptr;
            return dynamic_cast<T *>(it->second);
        }

        template<typename T,
                typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
        T *addComponent() {
            if (getComponent<T>() != nullptr) {
                return dynamic_cast<T *>(getComponent<T>());
            }
            T *val = new T;
            val->setParent(this);
            return dynamic_cast<T *>(m_components.insert(std::make_pair(std::type_index(typeid(T)), val))->second);
        }
    };
}


#endif //ENGINE_OBJECT_H
