//
// Created by Constantin on 19/07/2023.
//
#include "App.h"
#include "bx/math.h"
#include <stdio.h>
#include "../Rendering/Mesh.h"
#include "../Rendering/Mesh.h"
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

#include "GLFW/glfw3native.h"
#include "imgui/imgui.h"
#include <fstream>
#include <iostream>
#include "utils.h"
#include "Input.h"
#include "Data.h"
#include "../Components/CMeshRenderer.h"
#include "../Rendering/Renderer.h"
#include "ETime.h"
#include "../Components/CRigidBody.h"
#include "../Physic/Physic.h"
#include "Serialize.h"
#include "../UI/UIHierarchyWindow.h"
#include "../UI/UIDetailsWindow.h"
#include "../Components/CDummy.h"
#include "../Components/CParticleContainer.h"

static bool s_showStats = false;


static void glfw_errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
        s_showStats = !s_showStats;
}

namespace Engine {

    Engine::App::App(int width, int height, const char *title) : width(width), height(height) {
        w_name=title;
    }

    void Engine::App::init() {
        Log::Init();

        ENGINE_TRACE("Registering Components");
        initComponentType();

        ENGINE_TRACE("Init GLFW context");
        // Create a GLFW window without an OpenGL context.
        glfwSetErrorCallback(glfw_errorCallback);
        if (!glfwInit()) {
            ENGINE_FATAL("GLFW not initialized, exiting");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        currentWindow = glfwCreateWindow(width, height, w_name.c_str(), nullptr, nullptr);
        if (!currentWindow) {
            ENGINE_FATAL("Failed to create window, exiting");
            return;
        }

        ENGINE_TRACE("Init Data Manager");
        Data::init();
        ENGINE_TRACE("Init Input Manager");
        KeyInput::setupKeyInputs(currentWindow);

        input = std::make_unique<KeyInput>(std::vector<int>({GLFW_KEY_F1,GLFW_KEY_Z,GLFW_KEY_X, GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_2}));
        time = std::make_unique<ETime>();

        ENGINE_TRACE("Init rendering backend");
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
#ifdef USE_GL
        init.type = bgfx::RendererType::OpenGL;
#endif
        init.resolution.width = (uint32_t) width;
        init.resolution.height = (uint32_t) height;
        init.resolution.reset = BGFX_RESET_VSYNC;
        if (!bgfx::init(init)) {
            ENGINE_FATAL("Failed to initialize rendering backend, exiting");
            return;
        }

        ENGINE_TRACE("Init Renderer");
        renderer = new Renderer(width, height);

        m_timeOffset = bx::getHPCounter();
        ENGINE_TRACE("Init Editor Scene");
        editorScene = new Engine::Scene();

        ENGINE_TRACE("Init UI");
        initUI();
    }

    void Engine::App::run() {
        while (!glfwWindowShouldClose(currentWindow)) {
            glfwPollEvents();


            drawUI();

            time->processDelta();

            int oldWidth = width, oldHeight = height;
            glfwGetWindowSize(currentWindow, &width, &height);
            if (width != oldWidth || height != oldHeight) {
                renderer->setRect(width, height);
            }

            if (input->getIsKeyPressed(GLFW_KEY_F1)) {
                s_showStats = !s_showStats;
            }

            if (input->getIsKeyPressed(GLFW_KEY_Z)) {
                wireframe = !wireframe;
            }

            if (input->getIsKeyPressed(GLFW_KEY_X)) {
                renderer->toggleDrawDebugShape();
            }



            if (debugMode) {
                // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
                bgfx::touch(kClearView);
                // Use debug font to print information about this example.
                bgfx::dbgTextClear();
                bgfx::dbgTextPrintf(0, 0, 0x0f, "F1 : stats / W : wireframe / X : draw BBOX");
                const bgfx::Stats *stats = bgfx::getStats();
                bgfx::dbgTextPrintf(0, 1, 0x0f, "Delta Time : %f",
                                    time->getDeltaTime());
                bgfx::dbgTextPrintf(0, 2, 0x0f, "Draw call : %d",
                                    bgfx::getStats()->numDraw);
                // Enable stats or debug text.
                if (wireframe) {
                    bgfx::setDebug(BGFX_DEBUG_WIREFRAME);
                } else {
                    bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
                }
            }

            double fixedRate = 30;
            double fixedDelta = 1.0 / fixedRate;
            const int physicFrameCountLimit = 5;

            Component::processStart();
            lastFixedFrame += time->getDeltaTime();

            int pFCount = 0;
            while (lastFixedFrame >= fixedDelta && pFCount < physicFrameCountLimit) {
                Component::processFixedUpdate(fixedDelta);
                Physic::compute(fixedDelta);
                lastFixedFrame -= fixedDelta;
                pFCount++;
            }
            Component::processUpdate(time->getDeltaTime());
            Component::processLateUpdate(time->getDeltaTime());


            auto eye = currentCamera->getComponent<CTransform>()->getPosition();
            glm::vec3 at = currentCamera->getComponent<CTransform>()->getPosition() +
                           currentCamera->getComponent<CTransform>()->getForward();
            //at=bx::Vec3{0.0,0.0,0.0};

            auto view = glm::lookAt(eye, at, glm::vec3{0.0, 1.0, 0.0});

            renderer->setView(view);


            KeyInput::updateInputs();

            // Advance to next frame. Process submitted rendering primitives.
            renderer->render();
        }
    }

    void Engine::App::cleanup() {
        delete currentCamera;
        delete renderer;
    }

    Engine::App::~App() {
        bgfx::destroy(debugProgram);
        Data::cleanup();
        bgfx::shutdown();
        glfwTerminate();
    }

    void App::drawUI() {
        imguiBeginFrame(input->getMouseX(),
                        input->getMouseY(),
                        input->getIsKeyDown(
                                GLFW_MOUSE_BUTTON_1) || input->getIsKeyDown(GLFW_MOUSE_BUTTON_2),
                        input->getMouseScroll(),
                        (uint16_t) width,
                        (uint16_t) height,
                        KeyInput::getSingleCharKey()
        );

        //ImGui::ShowDemoWindow();

        for (auto window: editorWindows) {
            window->EditorUIDraw();
        }


        imguiEndFrame();

    }

    void App::initUI() {
        imguiCreate();


        ImGuiIO &io = ImGui::GetIO();
        io.WantCaptureKeyboard = true;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        auto hierarchy = new UI::UIHierarchyWindow(200, 400);
        ImGui::SetNextWindowPos({((float) (width - 500.0)), 200});
        auto details = new UI::UIDetailsWindow(400, 600);
        details->attachHierarchy(hierarchy);
        hierarchy->attachScene(editorScene);

        editorWindows.push_back(hierarchy);
        editorWindows.push_back(details);


    }

    void App::initComponentType() {
        REGISTER_COMPONENT(CTransform, "Transform")
        REGISTER_COMPONENT(CMeshRenderer, "Mesh Renderer")
        REGISTER_COMPONENT(CParticleContainer, "Particle Container")
        REGISTER_COMPONENT(CDummy, "Dummy Comp lol")
    }
}
