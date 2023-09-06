//
// Created by Constantin on 22/07/2023.
//

#ifndef ENGINE_ETIME_H
#define ENGINE_ETIME_H


namespace Engine {
    class ETime {
    public:
        ETime();

        void processDelta();

        double getDeltaTime();
        double lastFrame;
        double currentFrame;
        double deltaTime;
    };
}


#endif //ENGINE_ETIME_H
