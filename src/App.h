//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_APP_H
#define ENGINE_APP_H

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <bx/bx.h>
#include "Mesh.h"
#include "Components/CCamera.h"
#include "Input.h"
#include "Time.h"
#include "Objects/OCamera.h"
#include "Renderer.h"


namespace Engine {
    class App {
    public:
        App(int width, int height, const char *title);

        ~App();

        int init();

        void run();

        void cleanup();

    protected:
        bool debugMode = true;
        bool showStats = false;

        OCamera *currentCamera;

        Object *testMesh;

        KeyInput *input;
        std::unique_ptr<Time> time;
        Renderer *renderer;

        GLFWwindow *currentWindow;
        bgfx::ViewId kClearView;
        int width, height;
        int64_t m_timeOffset;

        //bgfx::ShaderHandle vsh;
        //bgfx::ShaderHandle fsh;
        bgfx::ProgramHandle program;
    };
}


#endif //ENGINE_APP_H
