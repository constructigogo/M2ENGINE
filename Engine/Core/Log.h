//
// Created by Constantin on 03/08/2023.
//

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE // Must: define SPDLOG_ACTIVE_LEVEL before `#include "spdlog/spdlog.h"`

#include "../CoreInc.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_sinks.h"

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

#define ENGINE_TRACE(...) SPDLOG_LOGGER_TRACE(Engine::Log::GetCoreLogger(),__VA_ARGS__)
#define ENGINE_INFO(...)  SPDLOG_LOGGER_INFO(Engine::Log::GetCoreLogger(),__VA_ARGS__)
#define ENGINE_WARN(...)  SPDLOG_LOGGER_WARN(Engine::Log::GetCoreLogger(),__VA_ARGS__)
#define ENGINE_ERROR(...) SPDLOG_LOGGER_ERROR(Engine::Log::GetCoreLogger(),__VA_ARGS__)
#define ENGINE_FATAL(...) SPDLOG_LOGGER_CRITICAL(Engine::Log::GetCoreLogger(),__VA_ARGS__)

#if NDEBUG
#define APP_DEBUG(...)
#define ENGINE_DEBUG(...)
#else
#define APP_DEBUG(...) SPDLOG_LOGGER_DEBUG(Engine::Log::GetClientLogger(),__VA_ARGS__)
#define ENGINE_DEBUG(...) SPDLOG_LOGGER_DEBUG(Engine::Log::GetClientLogger(),__VA_ARGS__)

#endif
#endif //ENGINE_LOG_H
