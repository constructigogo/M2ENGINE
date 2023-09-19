//
// Created by constantin on 14/09/23.
//

#include "UIHierarchyWindow.h"

namespace Engine {
    namespace UI {
        UIHierarchyWindow::UIHierarchyWindow(float width, float height) : UIWindow("Hierarchy",width, height) {
            flags=ImGuiWindowFlags_MenuBar;
        }

        void UIHierarchyWindow::attachScene(Scene *scene) {
            UIHierarchyWindow::attachedScene = scene;
        }

        void UIHierarchyWindow::EditorUIDraw() {
            UIWindow::EditorUIDraw();
        }

        Object *UIHierarchyWindow::getSelectedObject() const {
            return selectedObject;
        }

        void UIHierarchyWindow::setSelectedObject(Object *selectedObject) {
            UIHierarchyWindow::selectedObject = selectedObject;
        }

        void UIHierarchyWindow::EditorUIDrawHeader() {
            UIWindow::EditorUIDrawHeader();
        }

        void UIHierarchyWindow::EditorUIDrawContent() {
            UIWindow::EditorUIDrawContent();
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Object")) {
                    if (ImGui::MenuItem("New Empty Object", "Ctrl+O")) { attachedScene->createObject(); }
                    if (ImGui::MenuItem("empty", "Ctrl+S")) {}
                    if (ImGui::MenuItem("empty as well", "Ctrl+W")) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
                if (ImGui::BeginChild("Objet list")) {
                    int count = 0;
                    for (auto obj: attachedScene->getSceneObject()) {
                        ImGui::PushID(obj);
                        std::string label = obj->getName();
                        if (ImGui::Selectable(label.c_str(), selectedObject == obj, ImGuiSelectableFlags_None)) {
                            selectedObject = obj;
                        };
                        ImGui::PopID();
                        count++;
                    }
                }
                ImGui::EndChild();
            }
        }

        void UIHierarchyWindow::EditorUIDrawFoot() {
            UIWindow::EditorUIDrawFoot();
        }
    } // Engine
} // UI