//
// Created by Constantin on 22/07/2023.
//

#include "OCamera.h"
#include "../Components/CTransform.h"
#include "../Components/CCamera.h"

Engine::OCamera::OCamera() {
    addComponent<CTransform>();
    addComponent<CCamera>();
}
