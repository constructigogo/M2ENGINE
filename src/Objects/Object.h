//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <typeindex>
#include "map"
#include "memory"
#include "../Components/Component.h"



namespace Engine {
    class Component;
    class Object {
        Object * parent;
        std::multimap<std::type_index, Component*> m_components;
    public:

        Object()=default;
        ~Object();

        template<typename T,
                typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
        T* getComponent() {
            auto it = m_components.find(std::type_index(typeid(T)));
            if (it == m_components.end()) return nullptr;
            return dynamic_cast<T*>(it->second);
        }

        template<typename T,
                typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
        T* addComponent() {
            T* val = new T;
            val->setParent(this);
            return dynamic_cast<T*>(m_components.insert(std::make_pair(std::type_index(typeid(T)), val))->second);
        }

    };
}


#endif //ENGINE_OBJECT_H
