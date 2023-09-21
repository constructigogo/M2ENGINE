//
// Created by constantin on 14/09/23.
//

#pragma once

#include "UIWindow.h"
#include "../Core/Scene.h"


    namespace Engine::UI {

        class UIHierarchyWindow : public UIWindow {
        public:
            UIHierarchyWindow(float width, float height);

            void attachScene(Scene *scene);

            void EditorUIDraw() override;

            void EditorUIDrawHeader() override;

            void EditorUIDrawContent() override;

            void EditorUIDrawFoot() override;

            void HierarchyDrawObjectLine(Object * obj);


            Object *getSelectedObject() const;

            void setSelectedObject(Object *selectedObject);

            ~UIHierarchyWindow() override = default;

        protected:
            Scene *attachedScene;
            Object *selectedObject = nullptr;
            ImGuiTreeNodeFlags TreeBaseflags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            int node_clicked = -1;

        private:
        };

    } // Engine
// UI
