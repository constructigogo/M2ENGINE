//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_COMPONENT_H
#define ENGINE_COMPONENT_H

#include <vector>
#include <queue>
#include "../Objects/Object.h"
#include "../UI/EditorUIDraw.h"
#include "dear-imgui/imgui.h"
#include <set>


namespace Engine {
    class Object;

    class Component : public UI::EditorViewable {
    public:
        Component(Object *parent);

        virtual void start() {}

        virtual void fixedUpdate(double delta) {}

        virtual void update(double delta) {}

        virtual void lateUpdate(double delta) {}

        void EditorUIDraw() override;

        Object *getObject() const;

        void setParent(Object *_parent);

        bool isActive() const;

        void setIsActive(bool isActive);

        static std::set<std::type_index> types;

    private:
        Object *parent;

        bool _isActive = true;


    protected:
        Component();

        virtual void cleanup() {}

        void EditorUIDrawHeader() override;

        void EditorUIDrawContent() override;

        void EditorUIDrawFoot() override;


        ~Component() override;

        bool scheduledForStart;


        static std::queue<Component *> _startQueue;
    private:
        friend class App;

        friend class Object;

        static std::vector<Component *> _instances;

        static void processStart();

        static void processFixedUpdate(double delta);

        static void processUpdate(double delta);

        static void processLateUpdate(double delta);
    };
}


#endif //ENGINE_COMPONENT_H
