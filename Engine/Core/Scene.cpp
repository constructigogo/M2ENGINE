//
// Created by Constantin on 07/08/2023.
//

#include "Scene.h"
#include "../Components/CTransform.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../Components/CMeshRenderer.h"

namespace Engine {
    Object *Scene::createObject() {
        return createObject("Empty Object");
    }

    Object *Scene::createObject(std::string name) {
        auto ptr = new Object();
        ptr->setName(name);
        ptr->addComponent<CTransform>();
        sceneObject.push_back(ptr);
        return ptr;
    }

    const std::vector<Object *> &Scene::getSceneObject() const {
        return sceneObject;
    }

    void Scene::addObject(Object *toAdd) {
        if (toAdd->getComponent<CTransform>() == nullptr) {
            toAdd->addComponent<CTransform>();
        }
        sceneObject.push_back(toAdd);
    }

    void Scene::importScene(const std::string &Filename) {

    }
} // Engine