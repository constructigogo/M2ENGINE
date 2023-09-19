//
// Created by Constantin on 21/07/2023.
//
#include "Input.h"
#include <algorithm>
#include <iostream>
#include "dear-imgui/imgui_internal.h"
#include "dear-imgui/imgui.h"

std::vector<Engine::KeyInput *> Engine::KeyInput::_instances;
int Engine::KeyInput::lastKey = 0;
int Engine::KeyInput::lastKeyPrev = -1;
int Engine::KeyInput::lastScancode = 0;

std::array<int, GLFW_KEY_LAST> Engine::KeyInput::keyConvert;


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
    glfwSetScrollCallback(window, KeyInput::mouseScrollCallBack);
    glfwSetCharCallback(window, KeyInput::keyCharCallback);

    for (int &i: keyConvert) {
        i = 0;
    }
    {
        keyConvert[GLFW_KEY_ESCAPE] = ImGuiKey_Escape;
        keyConvert[GLFW_KEY_ENTER] = ImGuiKey_Enter;
        keyConvert[GLFW_KEY_TAB] = ImGuiKey_Tab;
        keyConvert[GLFW_KEY_SPACE] = ImGuiKey_Space;
        keyConvert[GLFW_KEY_BACKSPACE] = ImGuiKey_Backspace;
        keyConvert[GLFW_KEY_UP] = ImGuiKey_UpArrow;
        keyConvert[GLFW_KEY_DOWN] = ImGuiKey_DownArrow;
        keyConvert[GLFW_KEY_LEFT] = ImGuiKey_LeftArrow;
        keyConvert[GLFW_KEY_RIGHT] = ImGuiKey_RightArrow;
        keyConvert[GLFW_KEY_INSERT] = ImGuiKey_Insert;
        keyConvert[GLFW_KEY_DELETE] = ImGuiKey_Delete;
        keyConvert[GLFW_KEY_HOME] = ImGuiKey_Home;
        keyConvert[GLFW_KEY_END] = ImGuiKey_End;
        keyConvert[GLFW_KEY_PAGE_UP] = ImGuiKey_PageUp;
        keyConvert[GLFW_KEY_PAGE_DOWN] = ImGuiKey_PageDown;
        keyConvert[GLFW_KEY_PRINT_SCREEN] = ImGuiKey_PrintScreen;
        keyConvert[GLFW_KEY_EQUAL] = ImGuiKey_Equal;
        keyConvert[GLFW_KEY_MINUS] = ImGuiKey_Minus;
        keyConvert[GLFW_KEY_LEFT_BRACKET] = ImGuiKey_LeftBracket;
        keyConvert[GLFW_KEY_RIGHT_BRACKET] = ImGuiKey_RightBracket;
        keyConvert[GLFW_KEY_SEMICOLON] = ImGuiKey_Semicolon;
        keyConvert[GLFW_KEY_APOSTROPHE] = ImGuiKey_Apostrophe;
        keyConvert[GLFW_KEY_COMMA] = ImGuiKey_Comma;
        keyConvert[GLFW_KEY_PERIOD] = ImGuiKey_Period;
        keyConvert[GLFW_KEY_SLASH] = ImGuiKey_Slash;
        keyConvert[GLFW_KEY_BACKSLASH] = ImGuiKey_Backslash;
        keyConvert[GLFW_KEY_GRAVE_ACCENT] = ImGuiKey_GraveAccent;
        keyConvert[GLFW_KEY_F1] = ImGuiKey_F1;
        keyConvert[GLFW_KEY_F2] = ImGuiKey_F2;
        keyConvert[GLFW_KEY_F3] = ImGuiKey_F3;
        keyConvert[GLFW_KEY_F4] = ImGuiKey_F4;
        keyConvert[GLFW_KEY_F5] = ImGuiKey_F5;
        keyConvert[GLFW_KEY_F6] = ImGuiKey_F6;
        keyConvert[GLFW_KEY_F7] = ImGuiKey_F7;
        keyConvert[GLFW_KEY_F8] = ImGuiKey_F8;
        keyConvert[GLFW_KEY_F9] = ImGuiKey_F9;
        keyConvert[GLFW_KEY_F10] = ImGuiKey_F10;
        keyConvert[GLFW_KEY_F11] = ImGuiKey_F11;
        keyConvert[GLFW_KEY_F12] = ImGuiKey_F12;
        keyConvert[GLFW_KEY_KP_0] = ImGuiKey_Keypad0;
        keyConvert[GLFW_KEY_KP_1] = ImGuiKey_Keypad1;
        keyConvert[GLFW_KEY_KP_2] = ImGuiKey_Keypad2;
        keyConvert[GLFW_KEY_KP_3] = ImGuiKey_Keypad3;
        keyConvert[GLFW_KEY_KP_4] = ImGuiKey_Keypad4;
        keyConvert[GLFW_KEY_KP_5] = ImGuiKey_Keypad5;
        keyConvert[GLFW_KEY_KP_6] = ImGuiKey_Keypad6;
        keyConvert[GLFW_KEY_KP_7] = ImGuiKey_Keypad7;
        keyConvert[GLFW_KEY_KP_8] = ImGuiKey_Keypad8;
        keyConvert[GLFW_KEY_KP_9] = ImGuiKey_Keypad9;
        keyConvert[GLFW_KEY_0] = ImGuiKey_0;
        keyConvert[GLFW_KEY_1] = ImGuiKey_1;
        keyConvert[GLFW_KEY_2] = ImGuiKey_2;
        keyConvert[GLFW_KEY_3] = ImGuiKey_3;
        keyConvert[GLFW_KEY_4] = ImGuiKey_4;
        keyConvert[GLFW_KEY_5] = ImGuiKey_5;
        keyConvert[GLFW_KEY_6] = ImGuiKey_6;
        keyConvert[GLFW_KEY_7] = ImGuiKey_7;
        keyConvert[GLFW_KEY_8] = ImGuiKey_8;
        keyConvert[GLFW_KEY_9] = ImGuiKey_9;
        keyConvert[GLFW_KEY_A] = ImGuiKey_A;
        keyConvert[GLFW_KEY_B] = ImGuiKey_B;
        keyConvert[GLFW_KEY_C] = ImGuiKey_C;
        keyConvert[GLFW_KEY_D] = ImGuiKey_D;
        keyConvert[GLFW_KEY_E] = ImGuiKey_E;
        keyConvert[GLFW_KEY_F] = ImGuiKey_F;
        keyConvert[GLFW_KEY_G] = ImGuiKey_G;
        keyConvert[GLFW_KEY_H] = ImGuiKey_H;
        keyConvert[GLFW_KEY_I] = ImGuiKey_I;
        keyConvert[GLFW_KEY_J] = ImGuiKey_J;
        keyConvert[GLFW_KEY_K] = ImGuiKey_K;
        keyConvert[GLFW_KEY_L] = ImGuiKey_L;
        keyConvert[GLFW_KEY_M] = ImGuiKey_M;
        keyConvert[GLFW_KEY_N] = ImGuiKey_N;
        keyConvert[GLFW_KEY_O] = ImGuiKey_O;
        keyConvert[GLFW_KEY_P] = ImGuiKey_P;
        keyConvert[GLFW_KEY_Q] = ImGuiKey_Q;
        keyConvert[GLFW_KEY_R] = ImGuiKey_R;
        keyConvert[GLFW_KEY_S] = ImGuiKey_S;
        keyConvert[GLFW_KEY_T] = ImGuiKey_T;
        keyConvert[GLFW_KEY_U] = ImGuiKey_U;
        keyConvert[GLFW_KEY_V] = ImGuiKey_V;
        keyConvert[GLFW_KEY_W] = ImGuiKey_W;
        keyConvert[GLFW_KEY_X] = ImGuiKey_X;
        keyConvert[GLFW_KEY_Y] = ImGuiKey_Y;
        keyConvert[GLFW_KEY_Z] = ImGuiKey_Z;
    }

}


void Engine::KeyInput::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Send key event to all KeyInput instances
    ImGuiIO &io = ImGui::GetIO();
    io.AddKeyEvent((ImGuiKey) keyConvert[key], action != GLFW_RELEASE);

    if (action == GLFW_RELEASE) {
        lastKey = 0;
        lastScancode = 0;
    }
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


void Engine::KeyInput::mouseScrollCallBack(GLFWwindow *window, double xoffset, double yoffset) {
    for (Engine::KeyInput *keyInput: _instances) {
        keyInput->mouseScroll += yoffset;
    }
}

int Engine::KeyInput::getMouseScroll() {
    return mouseScroll;
}

void Engine::KeyInput::keyCharCallback(GLFWwindow *window, unsigned int codepoint) {
    {
        lastKey = codepoint;
        lastScancode = codepoint;
    }
}

unsigned int Engine::KeyInput::getSingleCharKey() {
    if (lastKey != lastKeyPrev) {
        lastKeyPrev = lastKey;
        return lastKey;
    } else {
        return 0;
    }
}










