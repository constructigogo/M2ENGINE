//
// Created by constantin on 14/09/23.
//

#ifndef ENGINE_EDITORUIDRAW_H
#define ENGINE_EDITORUIDRAW_H

#include "imgui_stdlib.h"

namespace Engine::UI {
    class EditorViewable {
    public:

        char* viewName;
        EditorViewable()=default;
        EditorViewable(char *name) {
            viewName = name;
        }

        virtual void EditorUIDraw()=0;


    protected:
        virtual void EditorUIDrawHeader()=0;
        virtual void EditorUIDrawContent()=0;
        virtual void EditorUIDrawFoot()=0;

        virtual ~EditorViewable() = default;
    };
}


#endif //ENGINE_EDITORUIDRAW_H
