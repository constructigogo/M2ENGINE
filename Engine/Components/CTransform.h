//
// Created by Constantin on 20/07/2023.
//

#ifndef ENGINE_CTRANSFORM_H
#define ENGINE_CTRANSFORM_H

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "memory"
#include "bx/math.h"
#include "Component.h"

enum CTransformMobility {
    STATIC,
    MOVABLE,
};

namespace Engine {

    class CRigidBody;

    class CTransform : public Component{
    private:

        friend CRigidBody;

        std::shared_ptr<CTransform*> Parent;

        glm::vec3 Position = glm::vec3(0.0f,0.0f,0.0f);
        glm::quat Rotation = glm::quat(1.0,0.0,0.0,0.0);
        glm::vec3 Scale = glm::vec3(1.0f,1.0f,1.0f);
    protected:
        void EditorUIDrawContent() override;

    public:
        CTransform();

        void setPosition(const glm::vec3 &position);
        void setPosition(const float &x,const float &y,const float &z);

        void setScale(const glm::vec3 &scale);
        void setScale(const float &x,const float &y,const float &z);
        void setScale(const float &xyz);

        void setRotation(const glm::quat &rotation);

        void translate(const glm::vec3 &position);
        void translate(const float &x,const float &y,const float &z);

        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;


        const glm::vec3 &getPosition() const;

        const glm::quat &getRotation() const;

        const glm::vec3 &getScale() const;

        glm::vec3 getPositionBX() const;
    };

}


#endif //ENGINE_CTRANSFORM_H
