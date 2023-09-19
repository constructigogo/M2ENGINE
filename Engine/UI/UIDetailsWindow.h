//
// Created by constantin on 14/09/23.
//

#pragma once

#include "UIWindow.h"
#include "UIHierarchyWindow.h"
#include "../Core/Serialize.h"

namespace Engine::UI {

#define COMPONENTSELECT(CLASS, TARG, NAME) \
    if(ImGui::Selectable(Serialize::getCompName(std::type_index(typeid(CLASS))).c_str())){ \
        TARG->addComponent<CLASS>();\
    }

    class UIDetailsWindow : public UIWindow {
    public:
        UIDetailsWindow(float width, float height);

        ~UIDetailsWindow() override = default;

        void attachHierarchy(UIHierarchyWindow *toAttach);

        void EditorUIDrawHeader() override;

        void EditorUIDraw() override;

        void EditorUIDrawContent() override;

        void EditorUIDrawFoot() override;


    protected:
        UIHierarchyWindow *hierarchyWindow = nullptr;


    private:
    };

} // Engine
// UI
