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
                    /*
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
                    */
                    for (auto obj: attachedScene->getSceneObject()) {
                        HierarchyDrawObjectLine(obj);
                    }


                }
                ImGui::EndChild();
            }
        }

        void UIHierarchyWindow::EditorUIDrawFoot() {
            UIWindow::EditorUIDrawFoot();
        }

        void UIHierarchyWindow::HierarchyDrawObjectLine(Object * obj) {
            ImGuiTreeNodeFlags node_flags = TreeBaseflags;
            int count = 0;
            ImGui::PushID(obj);
            auto childrens = obj->getChildrens();
            if(selectedObject == obj){
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }
            if (!childrens.empty()){
                //ImGui::PushID(count);
                bool node_open = ImGui::TreeNodeEx(obj, node_flags, "%s", obj->getName().c_str());
                if (ImGui::IsItemClicked()){
                    selectedObject = obj;
                    node_clicked = count++;
                }
                if (node_open){
                    for (auto &child :obj->getChildrens()) {
                        HierarchyDrawObjectLine(child);
                    }
                    ImGui::TreePop();
                }
            } else {
                ImGui::PushID(obj);
                std::string label = obj->getName();
                if (ImGui::Selectable(label.c_str(), selectedObject == obj, ImGuiSelectableFlags_None)) {
                    selectedObject = obj;
                };
                ImGui::PopID();
            }
            ImGui::PopID();
        }
    } // Engine
} // UI