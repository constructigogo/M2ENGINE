//
// Created by Constantin on 21/07/2023.
//

#ifndef ENGINE_CCAMERA_H
#define ENGINE_CCAMERA_H
#include "CTransform.h"
#include "Component.h"

namespace Engine {
    class CCamera : public Component{
    public :
        struct ViewData{
            float view[16];
            float proj[16];
        };

    private:
        CTransform * transform;
        ViewData data;

    public:
        CCamera();
        void start();
        void update(double deltaTime);

        const ViewData &getData() const;
        CTransform *getTransform() const;
    };
}


#endif //ENGINE_CCAMERA_H
