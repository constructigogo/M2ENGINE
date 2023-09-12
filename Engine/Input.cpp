//
// Created by Constantin on 21/07/2023.
//
#include "Input.h"
#include <algorithm>
#include <iostream>

std::vector<Engine::KeyInput *> Engine::KeyInput::_instances;

Engine::KeyInput::KeyInput(const std::vector<int> &keysToMonitor) : _isEnabled(true) {
    for (int key: keysToMonitor) {
        _keys[key].current = KEY_NOT_PRESSED;
        _keys[key].previous = KEY_NOT_PRESSED;
    }
    // Add this instance to the list of instances
    KeyInput::_instances.push_back(this);
}

Engine::KeyInput::~KeyInput() {
    // Remove this instance from the list of instances
    _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool Engine::KeyInput::getIsKeyDown(int key) {
    bool result = false;
    if (_isEnabled) {
        if (_keys.contains(key)) {
            result = _keys[key].current == KEY_PRESSED || _keys[key].current == KEY_HELD;
        }
    }
    return result;
}

bool Engine::KeyInput::getIsKeyPressed(int key) {
    bool result = false;
    if (_isEnabled) {
        if (_keys.contains(key)) {
            result = _keys[key].current == KEY_PRESSED;
        }
    }
    return result;
}

bool Engine::KeyInput::getIsKeyReleased(int key) {
    bool result = false;
    if (_isEnabled) {
        if (_keys.contains(key)) {
            result = _keys[key].current == KEY_RELEASED;
        }
    }
    return result;
}

void Engine::KeyInput::setupKeyInputs(GLFWwindow *window) {
    glfwSetKeyCallback(window, KeyInput::keyCallback);
    glfwSetCursorPosCallback(window, KeyInput::mousePosCallback);
    glfwSetMouseButtonCallback(window, KeyInput::mouseButtonCallback);
}


void Engine::KeyInput::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Send key event to all KeyInput instances
    for (Engine::KeyInput *keyInput: _instances) {
        Engine::KeyInput::KeyPress state = keyInput->getKeyState(key);
        if ((state.current == KEY_NOT_PRESSED || state.current == KEY_RELEASED) && action == GLFW_PRESS) {
            keyInput->setKeyState(key, KEY_PRESSED);
        } else if ((state.current == KEY_PRESSED || state.current == KEY_HELD) && action == GLFW_RELEASE) {
            keyInput->setKeyState(key, KEY_RELEASED);
        }
    }
}

Engine::KeyInput::KeyPress Engine::KeyInput::getKeyState(int key) {
    if (_keys.contains(key)) {
        return _keys[key];
    }
    return {-1, -1};
}

void Engine::KeyInput::setKeyState(int key, int state) {
    if (_keys.contains(key)) {
        _keys[key].previous = _keys[key].current;
        _keys[key].current = state;
    }
}

void Engine::KeyInput::updateInputs() {
    for (Engine::KeyInput *keyInput: _instances) {
        for (auto &key: keyInput->_keys) {
            if (key.second.current == KEY_PRESSED) {
                keyInput->setKeyState(key.first, KEY_HELD);
            } else if (key.second.current == KEY_RELEASED) {
                keyInput->setKeyState(key.first, KEY_NOT_PRESSED);
            }
        }
    }
}

int Engine::KeyInput::getMouseX() {
    return mouseX;
}

int Engine::KeyInput::getMouseY() {
    return mouseY;
}

glm::vec2 Engine::KeyInput::getMousePosition() {
    return {mouseX, mouseY};
}

void Engine::KeyInput::mousePosCallback(GLFWwindow *window, double xpos, double ypos) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    const double widthd = double(int32_t(w));
    const double heightd = double(int32_t(h));


    for (Engine::KeyInput *keyInput: _instances) {
        keyInput->mouseDeltaX = double(xpos - keyInput->mousePrevX) / widthd;
        keyInput->mouseDeltaY = double(ypos - keyInput->mousePrevY) / heightd;


        keyInput->mousePrevX = keyInput->mouseX;
        keyInput->mousePrevY = keyInput->mouseY;
        keyInput->mouseX = xpos;
        keyInput->mouseY = ypos;
    }
}

void Engine::KeyInput::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    // Send key event to all KeyInput instances
    for (Engine::KeyInput *keyInput: _instances) {
        Engine::KeyInput::KeyPress state = keyInput->getKeyState(button);
        if ((state.current == KEY_NOT_PRESSED || state.current == KEY_RELEASED) && action == GLFW_PRESS) {
            keyInput->setKeyState(button, KEY_PRESSED);
        } else if ((state.current == KEY_PRESSED || state.current == KEY_HELD) && action == GLFW_RELEASE) {
            keyInput->setKeyState(button, KEY_RELEASED);
        }
    }
}








