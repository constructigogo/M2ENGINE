//
// Created by Constantin on 22/07/2023.
//

#include "ETime.h"
#include "GLFW/glfw3.h"

void Engine::ETime::processDelta() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

Engine::ETime::ETime() : currentFrame(0), lastFrame(0), deltaTime(0){

}

double Engine::ETime::getDeltaTime() {
    return deltaTime;
}
