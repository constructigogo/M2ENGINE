//
// Created by constantin on 18/09/23.
//

#pragma once

#include <map>
#include <typeindex>
#include <string>
#include <functional>
#include "../Components/Component.h"
#include "../Components/CTransform.h"


namespace Engine {

    class Serialize {
    public:
        Serialize() = default;

        template<typename T>
        static void registerComponent(std::string str) {
            RegisteredComponents.emplace(std::type_index(typeid(T)),str);
        }

        static const std::map<std::type_index, std::string> &getRegisteredComponents() {
            return RegisteredComponents;
        }

        static std::string getCompName(std::type_index id){
            if(!RegisteredComponents.contains(id)){
                return std::string("");
            }
            return RegisteredComponents[id];
        }

    protected:
        static std::map<std::type_index, std::string> RegisteredComponents;
    private:
    };

#define REGISTER_COMPONENT(CLASS,name) \
    Serialize::registerComponent<CLASS>(name);

} // Engine
