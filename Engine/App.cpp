//
// Created by Constantin on 19/07/2023.
//
#include "App.h"
#include "bx/math.h"
#include <stdio.h>
#include "Mesh.h"
#include "Mesh.h"
#include "Log.h"
#include "entry/entry.h"
#include "bgfx/embedded_shader.h"
#include "bx/timer.h"

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>
#include "imgui/imgui.h"
#include <fstream>
#include <iostream>
#include "utils.h"
#include "Input.h"
#include "Data.h"
#include "Components/CMeshRenderer.h"
#include "Renderer.h"
#include "ETime.h"
#include "Components/CRigidBody.h"
#include "Physic.h"

static bool s_showStats = false;


static void glfw_errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

namespace Engine {

    Engine::App::App(int width, int height, const char *title) : width(width), height(height) {}

    void Engine::App::init() {

        Log::Init();

        // Create a GLFW window without an OpenGL context.
        glfwSetErrorCallback(glfw_errorCallback);
        if (!glfwInit())
            return;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        currentWindow = glfwCreateWindow(width, height, "don't look here", nullptr, nullptr);
        if (!currentWindow)
            return;

        Data::init();
        KeyInput::setupKeyInputs(currentWindow);

        input = new KeyInput({GLFW_KEY_F1, GLFW_MOUSE_BUTTON_1});
        time = std::make_unique<ETime>();

        //glfwSetKeyCallback(currentWindow, glfw_keyCallback);
        // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
        // Most graphics APIs must be used on the same thread that created the window.
        bgfx::renderFrame();
        // Initialize bgfx using the native window handle and window resolution.
        bgfx::Init init;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        init.platformData.ndt = glfwGetX11Display();
        init.platformData.nwh = (void *) (uintptr_t) glfwGetX11Window(currentWindow);
#elif BX_PLATFORM_OSX
        init.platformData.nwh = glfwGetCocoaWindow(currentWindow);
#elif BX_PLATFORM_WINDOWS
        init.platformData.nwh = glfwGetWin32Window(currentWindow);
#endif
        glfwGetWindowSize(currentWindow, &width, &height);
        init.type = bgfx::RendererType::OpenGL;
        init.resolution.width = (uint32_t) width;
        init.resolution.height = (uint32_t) height;
        init.resolution.reset = BGFX_RESET_VSYNC;
        if (!bgfx::init(init))
            return;

        renderer = new Renderer();


        // Set view 0 to the same dimensions as the window and to clear the color buffer.
        kClearView = 0;
        bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        //bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

        bgfx::setViewMode(kClearView, bgfx::ViewMode::Default);

        m_timeOffset = bx::getHPCounter();

        imguiCreate();
        //  return;
    }

    void Engine::App::run() {
        while (!glfwWindowShouldClose(currentWindow)) {
            glfwPollEvents();

            imguiBeginFrame(input->getMouseX(), input->getMouseY(), input->getIsKeyDown(GLFW_MOUSE_BUTTON_1), 0,
                            (uint16_t) width, (uint16_t) height);

            /*
            bool active = true;
            ImGui::SetNextWindowSize(
                    ImVec2(width / 5.0f, height / 3.5f)
                    , ImGuiCond_FirstUseEver
            );
            ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_MenuBar);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Object"))
                {
                    if (ImGui::MenuItem("New Empty Object", "Ctrl+O")) { }
                    if (ImGui::MenuItem("empty", "Ctrl+S"))   {  }
                    if (ImGui::MenuItem("empty as well", "Ctrl+W"))  { active = false; }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
*/
            imguiEndFrame();

            time->processDelta();

            int oldWidth = width, oldHeight = height;
            glfwGetWindowSize(currentWindow, &width, &height);
            if (width != oldWidth || height != oldHeight) {
                bgfx::reset((uint32_t) width, (uint32_t) height, BGFX_RESET_VSYNC);
                bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
            }

            if (input->getIsKeyPressed(GLFW_KEY_F1)) {
                s_showStats = !s_showStats;
            }

            if (debugMode) {
                // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
                bgfx::touch(kClearView);
                // Use debug font to print information about this example.
                bgfx::dbgTextClear();
                bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
                bgfx::dbgTextPrintf(0, 1, 0x0f,
                                    "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
                bgfx::dbgTextPrintf(80, 1, 0x0f,
                                    "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
                bgfx::dbgTextPrintf(80, 2, 0x0f,
                                    "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
                const bgfx::Stats *stats = bgfx::getStats();
                bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.",
                                    stats->width, stats->height, stats->textWidth, stats->textHeight);
                bgfx::dbgTextPrintf(0, 3, 0x0f, "Delta Time : %f",
                                    time->getDeltaTime());
                bgfx::dbgTextPrintf(0, 4, 0x0f, "Draw call : %d",
                                    bgfx::getStats()->numDraw);
                // Enable stats or debug text.
                bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
            }

            double fixedRate = 30;
            double fixedDelta = 1.0 / fixedRate;
            const int physicFrameCountLimit = 5;

            Component::processStart();
            lastFixedFrame += time->getDeltaTime();

            int pFCount = 0;
            while (lastFixedFrame >= fixedDelta && pFCount < physicFrameCountLimit) {
                Physic::compute(fixedDelta);
                Component::processFixedUpdate(fixedDelta);
                lastFixedFrame -= fixedDelta;
                pFCount++;
            }
            Component::processUpdate(time->getDeltaTime());
            Component::processLateUpdate(time->getDeltaTime());


            const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
            const bx::Vec3 eye = currentCamera->getComponent<CTransform>()->getPositionBX();
            float view[16];
            bx::mtxLookAt(view, eye, at);
            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 250.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(kClearView, view, proj);
            bgfx::setViewRect(kClearView, 0, 0, uint16_t(width), uint16_t(height));


            KeyInput::updateInputs();

            // Advance to next frame. Process submitted rendering primitives.
            renderer->render();
        }
    }

    void Engine::App::cleanup() {
        delete currentCamera;
        delete input;
    }

    Engine::App::~App() {
        bgfx::destroy(debugProgram);
        Data::cleanup();
        bgfx::shutdown();
        glfwTerminate();
    }
}
