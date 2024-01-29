//
// Created by Constantin on 07/08/2023.
//

#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <vector>
#include "../Objects/Object.h"
#include "HeightField.h"

namespace Engine {

    class Scene {
    public:
        Object *createObject();

        Object *createObject(std::string name);

        void addObject(Object *toAdd);

        void addObjects(std::vector<Object *> &toAdd);

        void setLandscapeAsSingle(const HeightField &hf,
                                  float scaleX = 1.0, float scaleY = 1.0, float scaleZ = 0.0,
                                  int resX = 0, int resY = 0);

        void setLandscapeAsSingle(const HeightField &hf, glm::vec3 scale, int resX = 0, int resY = 0);

        const std::vector<Object *> &getSceneObject() const;

    protected:
        std::vector<Object *> sceneObject;
    };

} // Engine

#endif //ENGINE_SCENE_H
