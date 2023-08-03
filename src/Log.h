//
// Created by Constantin on 03/08/2023.
//

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include "CoreInc.h"
#include "spdlog/spdlog.h"

namespace Engine {

    class Log {
        public:
            static void Init();
            inline static std::shared_ptr<spdlog::logger>& GetCoreLogger(){ return CoreLogger;};
            inline static std::shared_ptr<spdlog::logger>& GetClientLogger(){ return ClientLogger;};

    private:
            static std::shared_ptr<spdlog::logger> CoreLogger;
            static std::shared_ptr<spdlog::logger> ClientLogger;
    };

} // Engine

#endif //ENGINE_LOG_H
