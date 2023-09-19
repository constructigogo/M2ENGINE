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

#define LOG_TRACE(...) ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)  Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)  ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::Engine::Log::GetClientLogger()->fatal(__VA_ARGS__)

#endif //ENGINE_LOG_H
