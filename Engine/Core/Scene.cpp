//
// Created by Constantin on 07/08/2023.
//

#include "Scene.h"
#include "../Components/CTransform.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../Components/CMeshRenderer.h"
#include "../Rendering/MeshBuilder.h"
#include "Data.h"

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


    void Scene::addObjects(std::vector<Object *> &toAdd) {
        for (auto obj: toAdd) {
            addObject(obj);
        }
    }

    void Scene::setLandscapeAsSingle(const HeightField &hf, glm::vec3 scale, int resX, int resY) {
        static int uid = 0;
        if (resX <= 0) {
            resX = hf.gridWidth();
        }
        if (resY <= 0) {
            resY = hf.gridHeight();
        }

        if(scale.y <=0.0){
            scale.y = 1;
        }


        std::string name = "landscape" + std::to_string(uid++);
        auto msh = MeshBuilder::Polygonize(hf);
        msh->setName(name);
        Data::trackMesh(msh, name);
        auto inst = createObject("Landscape");

        auto transform = inst->getComponent<CTransform>();
        transform->setScale(scale);
        auto mRenderer = inst->addComponent<CMeshRenderer>()
                ->setMesh(msh, STATIC, false)
                ->setMaterial(Material::Default.createInstance());
    }

    void
    Scene::setLandscapeAsSingle(const HeightField &hf, float scaleX, float scaleY, float scaleZ, int resX, int resY) {
        setLandscapeAsSingle(hf, {scaleX, scaleZ, scaleY}, resX, resY);
    }
} // Engine