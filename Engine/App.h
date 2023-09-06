//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_APP_H
#define ENGINE_APP_H

#include "CoreInc.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#include <bx/bx.h>
#include "Mesh.h"
#include "Components/CCamera.h"
#include "Input.h"
#include "ETime.h"
#include "Objects/OCamera.h"
#include "Renderer.h"


namespace Engine {

    class Physic;

    class App {
    public:
        explicit App(int width=1024, int height= 768, const char *title="blabla");

        virtual ~App();

        virtual void init();

        virtual void run();

        virtual void cleanup();

    protected:
        bool debugMode = true;
        bool showStats = false;

        OCamera *currentCamera;

        Object *testMesh;

        KeyInput *input;
        std::unique_ptr<ETime> time;
        Renderer *renderer;

        GLFWwindow *currentWindow;
        bgfx::ViewId kClearView;
        int width, height;
        int64_t m_timeOffset;

        double lastFixedFrame =0;

        //bgfx::ShaderHandle vsh;
        //bgfx::ShaderHandle fsh;
        bgfx::ProgramHandle debugProgram;
    };

    App* CreateApp();

}


#endif //ENGINE_APP_H
