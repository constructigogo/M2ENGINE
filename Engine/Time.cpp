//
// Created by Constantin on 22/07/2023.
//

#include "Time.h"
#include "GLFW/glfw3.h"

void Engine::Time::processDelta() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

Engine::Time::Time() : currentFrame(0), lastFrame(0), deltaTime(0){

}

double Engine::Time::getDeltaTime() {
    return deltaTime;
}
