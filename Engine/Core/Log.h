//
// Created by Constantin on 03/08/2023.
//

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include "../CoreInc.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Engine {

    class Log {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return CoreLogger; };

        inline static std::shared_ptr<spdlog::logger> &GetClientLogger() { return ClientLogger; };

    private:
        static std::shared_ptr<spdlog::logger> CoreLogger;
        static std::shared_ptr<spdlog::logger> ClientLogger;
    };

} // Engine

#define ENGINE_TRACE(...) Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)  Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)  Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_FATAL(...) Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#if NDEBUG
    #define APP_DEBUG(...)
    #define ENGINE_DEBUG(...)
#else
    #define APP_DEBUG(...) Engine::Log::GetClientLogger()->debug(__VA_ARGS__)
    #define ENGINE_DEBUG(...) Engine::Log::GetCoreLogger()->debug(__VA_ARGS__)

#endif
#endif //ENGINE_LOG_H
