//
// Created by constantin on 14/09/23.
//

#pragma once

#include "EditorUIDraw.h"
#include "dear-imgui/imgui.h"

namespace Engine::UI {

    class UIWindow : public EditorViewable {
    public:
        UIWindow() = delete;

        UIWindow(char * name,float width, float height);

        void EditorUIDraw() override;

        ~UIWindow() override = default;

    protected:
        void EditorUIDrawHeader() override;

        void EditorUIDrawContent() override;

        void EditorUIDrawFoot() override;


        float width;
        float height;
        ImGuiWindowFlags flags=0;
    private:
    };

} // Engine
// UI
