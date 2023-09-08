//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_COMPONENT_H
#define ENGINE_COMPONENT_H

#include <vector>
#include <queue>
#include "../Objects/Object.h"



namespace Engine {
    class Object;
    class Component {
    public:
        Component(Object * parent);
        virtual void start() {}
        virtual void fixedUpdate(double delta) {}
        virtual void update(double delta) {}
        virtual void lateUpdate(double delta) {}

        Object *getParent() const;

    private:
        Object *parent;


    protected:
        Component();

        virtual void cleanup(){}
        virtual ~Component();
        bool scheduledForStart;


        static std::queue<Component *> _startQueue;
    private:
        friend class App;
        friend class Object;

        void setParent(Object * _parent);

        static std::vector<Component *> _instances;

        static void processStart();
        static void processFixedUpdate(double delta);
        static void processUpdate(double delta);
        static void processLateUpdate(double delta);
    };
}


#endif //ENGINE_COMPONENT_H
