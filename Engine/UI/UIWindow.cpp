//
// Created by constantin on 14/09/23.
//

#include "UIWindow.h"
#include "dear-imgui/imgui.h"


namespace Engine::UI {
    void UIWindow::EditorUIDraw() {
        EditorUIDrawHeader();
        EditorUIDrawContent();
        EditorUIDrawFoot();
    }

    UIWindow::UIWindow(char* name,float width, float height) : EditorViewable(name), width(width), height(height) {}

    void UIWindow::EditorUIDrawHeader() {
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_FirstUseEver);
        ImGui::Begin(viewName, nullptr, flags);

    }

    void UIWindow::EditorUIDrawContent() {

    }

    void UIWindow::EditorUIDrawFoot() {
        ImGui::End();
    }
} // Engine
// UI