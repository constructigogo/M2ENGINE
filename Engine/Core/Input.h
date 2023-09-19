//
// Created by Constantin on 21/07/2023.
//

#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H


#include <vector>
#include <map>
#include "../Singleton.h"
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

#define KEY_NOT_PRESSED 0
#define KEY_PRESSED 1
#define KEY_HELD 2
#define KEY_RELEASED 3

namespace Engine {
    class KeyInput {
        // Main KeyInput functionality
    public:

        struct KeyPress {
            int8_t current, previous;
        };

        // Takes a list of which keys to keep state for
        KeyInput(const std::vector<int> &keysToMonitor);

        ~KeyInput();

        glm::vec2 getMousePosition();

        int getMouseX();
        int getMouseY();

        int getMouseScroll();
        // If this KeyInput is enabled and the given key is monitored,
        // returns pressed state.  Else returns false.
        bool getIsKeyDown(int key);

        bool getIsKeyPressed(int key);

        bool getIsKeyReleased(int key);

        // See _isEnabled for details
        bool getIsEnabled() { return _isEnabled; }

        void setIsEnabled(bool value) { _isEnabled = value; }

    private:
        // Used internally to update key states.  Called by the GLFW callback.
        KeyPress getKeyState(int key);

        void setKeyState(int key, int state);

        // Map from monitored keyes to their pressed states
        std::map<int, KeyPress> _keys;
        // If disabled, KeyInput.getIsKeyDown always returns false
        bool _isEnabled;

        double mouseX;
        double mouseY;
        double mousePrevX = 0.0;
        double mousePrevY = 0.0;
        double mouseDeltaX;
        double mouseDeltaY;
        double mouseScroll=0;
        double mouseScrollDelta;



        // Workaround for C++ class using a c-style-callback
    public:
        // Must be called before any KeyInput instances will work
        static void setupKeyInputs(GLFWwindow *window);

        static void updateInputs();
        static unsigned int getSingleCharKey();

        static int lastKey;
        static int lastKeyPrev;
        static int lastScancode;

    private:
        // The GLFW callback for key events.  Sends events to all KeyInput instances
        static void keyCallback(
                GLFWwindow *window, int key, int scancode, int action, int mods);
        static void keyCharCallback(GLFWwindow* window, unsigned int codepoint);

        static void mousePosCallback(GLFWwindow *window, double xpos, double ypos);
        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void mouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);

        // Keep a list of all KeyInput instances and notify them all of key events
        static std::vector<KeyInput *> _instances;

        static std::array<int,GLFW_KEY_LAST> keyConvert;



    };
}


#endif //ENGINE_INPUT_H
