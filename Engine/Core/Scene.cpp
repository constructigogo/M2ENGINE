//
// Created by Constantin on 07/08/2023.
//

#include "Scene.h"
#include "../Components/CTransform.h"

namespace Engine {
    Object *Scene::createObject() {
        return createObject("Empty Object");
    }

    Object *Scene::createObject(char *name) {
        auto ptr = new Object();
        ptr->addComponent<CTransform>();
        sceneObject.push_back(ptr);
        return ptr;
    }

    const std::vector<Object *> &Scene::getSceneObject() const {
        return sceneObject;
    }

    void Scene::addObject(Object *toAdd) {
        if(toAdd->getComponent<CTransform>()== nullptr){
            toAdd->addComponent<CTransform>();
        }
        sceneObject.push_back(toAdd);
    }
} // Engine