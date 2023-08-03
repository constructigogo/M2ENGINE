//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_TIME_H
#define ENGINE_TIME_H


namespace Engine {
    class Time {
    public:
        Time();

        void processDelta();

        double getDeltaTime();
        double lastFrame;
        double currentFrame;
        double deltaTime;
    };
}


#endif //ENGINE_TIME_H
