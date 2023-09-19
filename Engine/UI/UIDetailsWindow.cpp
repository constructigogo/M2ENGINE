//
// Created by constantin on 14/09/23.
//

#include "UIDetailsWindow.h"
#include "iostream"
#include "set"
#include <memory>
#include "../Core/Serialize.h"
#include "../Components/CDummy.h"
#include "../Components/CMeshRenderer.h"

namespace Engine::UI {
    UIDetailsWindow::UIDetailsWindow(float width, float height) : UIWindow("Details",width, height) {}

    void UIDetailsWindow::attachHierarchy(UIHierarchyWindow *toAttach) {
        hierarchyWindow = toAttach;
    }

    void UIDetailsWindow::EditorUIDrawHeader() {
        UIWindow::EditorUIDrawHeader();
    }

    void UIDetailsWindow::EditorUIDraw() {
        UIWindow::EditorUIDraw();
    }

    void UIDetailsWindow::EditorUIDrawFoot() {
        UIWindow::EditorUIDrawFoot();
    }

    void UIDetailsWindow::EditorUIDrawContent() {
        UIWindow::EditorUIDrawContent();
        auto target = hierarchyWindow->getSelectedObject();
        if (target != nullptr) {
            bool isActive = target->isActive();
            if (ImGui::Checkbox("Active", &isActive)){
                target->setIsActive(isActive);
            }
            ImGui::SameLine();
            ImGui::PushID(target);
            std::string buff = target->getName();
            if (ImGui::InputText("##namechange",&buff,ImGuiInputTextFlags_EnterReturnsTrue)){
                target->setName(buff);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::Text("UID : %lu",target->getUid());


            ImGui::Separator();
            std::set<std::type_index> owned_types;
            for (auto [key, comp]: target->getComponents()) {
                ImGui::PushID(comp);
                comp->EditorUIDraw();
                ImGui::PopID();
                owned_types.emplace(key);
            }

            ImGui::Separator();

            if(ImGui::Button("Add Component")){
                ImGui::OpenPopup("add_comp_popup");
            }

            if (ImGui::BeginPopup("add_comp_popup"))
            {
                ImGui::Text("Components");
                ImGui::Separator();

                COMPONENTSELECT(CTransform,target,"Transform")
                COMPONENTSELECT(CMeshRenderer,target,"Mesh Renderer")
                COMPONENTSELECT(CDummy,target,"dummy")

                ImGui::EndPopup();
            }
        }
    }

} // Engine
// UI