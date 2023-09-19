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

        bx::Vec3 Position = bx::Vec3(bx::InitZero);
        bx::Quaternion Rotation = bx::Quaternion(bx::InitZero);
        bx::Vec3 Scale = bx::Vec3(1.0f,1.0f,1.0f);

    protected:
        void EditorUIDrawContent() override;

    public:
        CTransform();
        CTransform(const bx::Vec3 &Pos);
        CTransform(const bx::Quaternion &Rot);
        CTransform(const bx::Vec3 &Pos, const std::vector<glm::quat> &Rot);

        void setPosition(const bx::Vec3 &position);
        void setPosition(const float &x,const float &y,const float &z);

        void setScale(const bx::Vec3 &scale);
        void setScale(const float &x,const float &y,const float &z);
        void setScale(const float &xyz);

        void setRotation(const bx::Quaternion &rotation);

        void translate(const bx::Vec3 &position);
        void translate(const glm::vec3 &position);
        void translate(const float &x,const float &y,const float &z);

        const bx::Vec3 &getPosition() const;

        const bx::Quaternion &getRotation() const;

        const bx::Vec3 &getScale() const;

        bx::Vec3 getPositionBX() const;
    };

}


#endif //ENGINE_CTRANSFORM_H
