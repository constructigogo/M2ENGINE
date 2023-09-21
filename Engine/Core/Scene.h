//
// Created by Constantin on 07/08/2023.
//

#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <vector>
#include "../Objects/Object.h"
namespace Engine {

    class Scene {
    public:
        Object * createObject();
        Object * createObject(std::string name);

        void importScene(const std::string &Filename);

        void addObject(Object * toAdd);

        const std::vector<Object *> &getSceneObject() const;

    protected:
        std::vector<Object*> sceneObject;
    };

} // Engine

#endif //ENGINE_SCENE_H
